#include "datainfotest.h"
#include "qmetaobject.h"
#include <gtest/gtest.h>

TEST_F(DataInfoTest, typeRegistrationTest)
{
    Systemus::DataInfo info;

    info = Systemus::DataInfo::fromName("SampleData");
    ASSERT_NE(info.tableName(), "Samples");

    sRegisterType<SampleData>();

    info = Systemus::DataInfo::fromName("SampleData");
    ASSERT_EQ(info.tableName(), "Samples");
}

TEST_F(DataInfoTest, dataInfoGathering)
{
    const QMetaObject *metaObject = &SampleData::staticMetaObject;
    QSqlField field;

    Systemus::DataInfo info = Systemus::DataInfo::fromType<SampleData>();

    // Table name check
    ASSERT_EQ(info.tableName(), "Samples");

    // ID property check

    ASSERT_EQ(info.idPropertyName(), "id");
    ASSERT_EQ(info.idProperty(), property("id", metaObject));
    ASSERT_EQ(info.idFieldName(), "id");
    field = info.idField();
    ASSERT_EQ(field.name(), "id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    // User property check

    ASSERT_EQ(info.userPropertyName(), "name");
    ASSERT_EQ(info.userProperty(), property("name", metaObject));
    field = info.userField();
    ASSERT_EQ(field.name(), "name");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    // Display properties check

    ASSERT_EQ(info.displayCount(), 3);

    ASSERT_EQ(info.displayPropertyName(0), "name");
    ASSERT_EQ(info.displayProperty(0), property("name", metaObject));
    ASSERT_EQ(info.displayFieldName(0), "name");
    field = info.displayField(0);
    ASSERT_EQ(field.name(), "name");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.displayPropertyName(1), "description");
    ASSERT_EQ(info.displayProperty(1), property("description", metaObject));
    ASSERT_EQ(info.displayFieldName(1), "description");
    field = info.displayField(1);
    ASSERT_EQ(field.name(), "description");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.displayPropertyName(2), "count");
    ASSERT_EQ(info.displayProperty(2), property("count", metaObject));
    ASSERT_EQ(info.displayFieldName(2), "count");
    field = info.displayField(2);
    ASSERT_EQ(field.name(), "count");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    // Search properties check

    ASSERT_EQ(info.searchPropertyNames(), QStringList() << "name" << "description");
    ASSERT_EQ(info.searchFieldNames(), QStringList() << "name" << "description");

    // Secret properties check

    ASSERT_EQ(info.secretPropertyNames(), QStringList() << "secretId");
    ASSERT_EQ(info.secretFieldNames(), QStringList() << "secret_id");

    // Global properties check

    ASSERT_EQ(info.count(), 5);

    ASSERT_EQ(info.fieldPropertyName(0), "id");
    ASSERT_EQ(info.fieldProperty(0), property("id", metaObject));
    ASSERT_EQ(info.fieldName(0), "id");
    field = info.field(0);
    ASSERT_EQ(field.name(), "id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.fieldPropertyName(1), "name");
    ASSERT_EQ(info.fieldProperty(1), property("name", metaObject));
    ASSERT_EQ(info.fieldName(1), "name");
    field = info.field(1);
    ASSERT_EQ(field.name(), "name");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.fieldPropertyName(2), "description");
    ASSERT_EQ(info.fieldProperty(2), property("description", metaObject));
    ASSERT_EQ(info.fieldName(2), "description");
    field = info.field(2);
    ASSERT_EQ(field.name(), "description");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.fieldPropertyName(3), "count");
    ASSERT_EQ(info.fieldProperty(3), property("count", metaObject));
    ASSERT_EQ(info.fieldName(3), "count");
    field = info.field(3);
    ASSERT_EQ(field.name(), "count");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.fieldPropertyName(4), "secretId");
    ASSERT_EQ(info.fieldProperty(4), property("secretId", metaObject));
    ASSERT_EQ(info.fieldName(4), "secret_id");
    field = info.field(4);
    ASSERT_EQ(field.name(), "secret_id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    // Foreign property check

    ASSERT_EQ(info.foreignPropertyName(), "sampleId");
    ASSERT_EQ(info.foreignFieldName(), "sample_id");
    field = info.foreignField();
    ASSERT_EQ(field.name(), "sample_id");
    ASSERT_EQ(field.metaType(), info.idField().metaType());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);
}
