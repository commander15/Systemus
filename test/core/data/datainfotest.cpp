#include "datainfotest.h"
#include <gtest/gtest.h>

TEST_F(DataInfoTest, typeRegistrationTest)
{
    Systemus::DataInfo info;

    info = Systemus::DataInfo::fromRegistry("SampleData");
    ASSERT_NE(info.tableName(), "Samples");

    sRegisterType<SampleData>();

    info = Systemus::DataInfo::fromRegistry("SampleData");
    ASSERT_EQ(info.tableName(), "Samples");
}

TEST_F(DataInfoTest, dataInfoGathering)
{
    Systemus::DataInfo info = sample.dataInfo();

    ASSERT_EQ(info.idField(), info.field(0));
    ASSERT_EQ(info.userField(), info.field(1));
    ASSERT_EQ(info.foreignFieldName(), "sample_id");
    ASSERT_EQ(info.searchFieldNames(), QStringList() << "name" << "description");

    ASSERT_EQ(info.fieldCount(), 5);

    QSqlField field;

    field = info.field(0);
    ASSERT_EQ(field.name(), "id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());

    field = info.field(1);
    ASSERT_EQ(field.name(), "name");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());

    field = info.field(2);
    ASSERT_EQ(field.name(), "description");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<QString>());

    field = info.field(3);
    ASSERT_EQ(field.name(), "count");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());

    field = info.field(4);
    ASSERT_EQ(field.name(), "secret_id");
    ASSERT_EQ(field.metaType(), QMetaType::fromType<int>());
    ASSERT_EQ(field.requiredStatus(), QSqlField::Required);

    ASSERT_EQ(info.tableName(), "Samples");
}
