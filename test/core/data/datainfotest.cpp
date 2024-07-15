#include "datainfotest.h"
#include "qmetaobject.h"
#include <gtest/gtest.h>

#include <QtSql/qsqlfield.h>

TEST_F(DataInfoTest, typeRegistrationTest)
{
    Systemus::DataInfo info;

    sRegisterType<SampleData>();

    info = Systemus::DataInfo::fromName("SampleData");
    ASSERT_EQ(info.tableName(), "Samples");
}

TEST_F(DataInfoTest, dataInfoGathering)
{
    Systemus::DataInfo info = Systemus::DataInfo::fromType<SampleData>();

    ASSERT_EQ(info.idPropertyName(), "id");
    ASSERT_EQ(std::string(info.idProperty().name()), "id");

    ASSERT_EQ(info.userPropertyName(), "name");
    ASSERT_EQ(std::string(info.userProperty().name()), "name");

    ASSERT_EQ(info.displayCount(), 3);

    ASSERT_EQ(info.displayPropertyName(0), "name");
    ASSERT_EQ(std::string(info.displayProperty(0).name()), "name");

    ASSERT_EQ(info.displayPropertyName(1), "description");
    ASSERT_EQ(std::string(info.displayProperty(1).name()), "description");

    ASSERT_EQ(info.displayPropertyName(2), "count");
    ASSERT_EQ(std::string(info.displayProperty(2).name()), "count");

    ASSERT_EQ(info.count(), 5);

    ASSERT_EQ(info.fieldPropertyName(0), "id");
    ASSERT_EQ(std::string(info.fieldProperty(0).name()), "id");

    ASSERT_EQ(info.fieldPropertyName(1), "name");
    ASSERT_EQ(std::string(info.fieldProperty(1).name()), "name");

    ASSERT_EQ(info.fieldPropertyName(2), "description");
    ASSERT_EQ(std::string(info.fieldProperty(2).name()), "description");

    ASSERT_EQ(info.fieldPropertyName(3), "count");
    ASSERT_EQ(std::string(info.fieldProperty(3).name()), "count");

    ASSERT_EQ(info.fieldPropertyName(4), "secretId");
    ASSERT_FALSE(info.fieldProperty(4).isValid());

    QSqlField field;

    ASSERT_EQ(info.fieldName(0), "id");
    field = info.field(0);
    ASSERT_EQ(field.name(), "id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    field = info.field(1);
    ASSERT_EQ(field.name(), "name");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    field = info.field(2);
    ASSERT_EQ(field.name(), "description");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    field = info.field(3);
    ASSERT_EQ(field.name(), "count");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_NE(field.requiredStatus(), QSqlField::Required);

    field = info.field(4);
    ASSERT_EQ(field.name(), "secret_id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.tableName(), "Samples");
}
