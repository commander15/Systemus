#include "datatest.h"

#include <QtCore/qjsonobject.h>

TEST_F(DataTest, transportationTest)
{
    sample.setId(1);
    sample.description = "Bla bla";
    sample.sampleCount = 15;
    sample.setProperty("secretId", 11);

    sample.name = "Hello";
    ASSERT_EQ(sample.property("name"), "Hello");

    sample.setProperty("name", "World");
    ASSERT_EQ(sample.name, "World");

    sample.setProperty("secretId", 12);
    ASSERT_EQ(sample.property("secretId").toInt(), 12);

    Systemus::Data data = sample;
    ASSERT_EQ(data.property("name"), "World");
    data.setProperty("name", "Geek");

    SampleData sample2 = data.to<SampleData>();

    ASSERT_EQ(sample2.property("name").toString().toStdString(), "Geek");
}

TEST_F(DataTest, simpleJsonGenerationTest)
{
    sample.setId(10);
    sample.name = "HIV";
    sample.description = "HIV/AIDS";
    sample.sampleCount = 11;

    QJsonObject object = sample.toJsonObject();
    ASSERT_EQ(object.value("id").toInt(), 10);
    ASSERT_EQ(object.value("name").toString().toStdString(), "HIV");
    ASSERT_EQ(object.value("description").toString(), "HIV/AIDS");
    ASSERT_EQ(object.value("count").toInt(), 11);
}

TEST_F(DataTest, simpleGenericJsonGenerationTest)
{
    sample.setId(10);
    sample.name = "HIV";
    sample.description = "HIV/AIDS";
    sample.sampleCount = 11;

    Systemus::Data data = sample;

    QJsonObject object = data.toJsonObject();
    ASSERT_EQ(object.value("id").toInt(), 10);
    ASSERT_EQ(object.value("name").toString().toStdString(), "HIV");
    ASSERT_EQ(object.value("description").toString(), "HIV/AIDS");
    ASSERT_EQ(object.value("count").toInt(), 11);
}

TEST_F(DataTest, advancedJsonGenerationTest)
{
    sRegisterType<SensitiveSample>()
        .init([] {
            sRegisterType<SampleData>();
        })
        .transfer([](const Systemus::Data *from, Systemus::Data *to, int type) {
            if (type == 0) {
                const SensitiveSample *sample = static_cast<const SensitiveSample *>(from);
                to->setProperty("ref", QVariant::fromValue(sample->ref.data()));
            } else if (type == 1) {
                auto *sample = static_cast<SensitiveSample *>(to);
                sample->ref.setData(from->property("ref").value<SampleData>());
            }
        })
        .jsonGenerator([](const Systemus::Data &data, QJsonObject *object) {
            if (true) {
                const SensitiveSample &sample = static_cast<const SensitiveSample &>(data);
                object->insert("ref_sample", sample.ref.toJson());
            } else {
                object->insert("ref_sample", data.property("refSample").value<SampleData>().toJsonObject());
            }
        });

    SensitiveSample sample;
    sample.setId(10);
    sample.name = "HIV";
    sample.description = "HIV/AIDS";
    sample.sampleCount = 11;
    sample.riskLevel = 9;
    sample.ref->setId(23);
    sample.ref->name = "X234";
    sample.ref->description = "Base virus of HIV/AIDS";

    QJsonObject object = sample.toJsonObject();
    ASSERT_EQ(object.value("id").toInt(), 10);
    ASSERT_EQ(object.value("name").toString().toStdString(), "HIV");
    ASSERT_EQ(object.value("description").toString(), "HIV/AIDS");
    ASSERT_EQ(object.value("count").toInt(), 11);
    ASSERT_TRUE(object.contains("ref_sample"));
}
