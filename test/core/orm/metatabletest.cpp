#include "metatabletest.h"

#include <QtSql/qsqlfield.h>

#define QSTRING(str) str.toStdString()
#define STRING(str)  std::string(str)

using namespace Systemus::Orm;

TEST_F(MetaTableTest, metaTableBuildTest)
{
    const MetaTable table = MetaTable::fromType<SimpleSample>();

    // Class name test (just for check)
    ASSERT_EQ(table.className().toStdString(), "SimpleSample");

    ASSERT_EQ(table.metaCount(), 2);
    {
        QMetaProperty property;

        property = table.metaProperty(0);
        ASSERT_EQ(STRING(property.name()), "id");
        ASSERT_EQ(property.metaType().id(), QMetaType::fromType<int>().id());

        property = table.metaProperty(1);
        ASSERT_EQ(STRING(property.name()), "name");
        ASSERT_EQ(property.metaType().id(), QMetaType::fromType<QString>().id());
    }

    // Meta properties test (to check if only stored properties are included and they are all there)
    ASSERT_EQ(table.metaCount(), 2);
    ASSERT_EQ(STRING(table.metaProperty(0).name()), "id");
    ASSERT_EQ(STRING(table.metaProperty(1).name()), "name");

    // Secret properties test
    ASSERT_EQ(table.secretCount(), 1);
    {
        SecretProperty property;

        property = table.secretProperty(0);
        ASSERT_EQ(STRING(property.name()), "secretLevel");
        ASSERT_EQ(property.metaType().id(), QMetaType::fromType<int>().id());
        ASSERT_EQ(property.isRequired(), true);
    }

    // whole object
    ASSERT_EQ(table.count(), 3);

    // property testing
    SecretProperty property;

    property = table.property(0);
    ASSERT_EQ(STRING(property.name()), "id");
    ASSERT_EQ(property.metaType().id(), QMetaType::fromType<int>().id());

    property = table.property(1);
    ASSERT_EQ(STRING(property.name()), "name");
    ASSERT_EQ(property.metaType().id(), QMetaType::fromType<QString>().id());

    property = table.property(2);
    ASSERT_EQ(STRING(property.name()), "secretLevel");
    ASSERT_EQ(property.metaType().id(), QMetaType::fromType<int>().id());

    // Table name test
    ASSERT_EQ(QSTRING(table.tableName()), "SimpleSamples");

    // fields testing
    QSqlField field;

    // primary field
    field = table.primaryField();
    ASSERT_EQ(QSTRING(field.name()), "id");

    // user field
    field = table.userField();
    ASSERT_EQ(QSTRING(field.name()), "name");

    // whole record
    QSqlRecord record = table.record();
    ASSERT_EQ(record.count(), 3);

    field = record.field(0);
    ASSERT_EQ(QSTRING(field.name()), "id");

    field = record.field(1);
    ASSERT_EQ(QSTRING(field.name()), "name");

    field = record.field(2);
    ASSERT_EQ(QSTRING(field.name()), "secret_level");
}

TEST_F(MetaTableTest, indexingTest)
{
    const MetaTable table = MetaTable::fromType<SimpleSample>();

    ASSERT_EQ(table.indexOfProperty("id"), 0);
    ASSERT_EQ(table.indexOfProperty("name"), 1);
    ASSERT_EQ(table.indexOfProperty("secretLevel"), 2);
}
