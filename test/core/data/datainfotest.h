#ifndef DATAINFOTEST_H
#define DATAINFOTEST_H

#include <gtest/gtest.h>

#include <SystemusCore/data.h>

#include <QtSql/qsqlfield.h>

#include <QtCore/qmetaobject.h>

class SampleData : public Systemus::Data
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(int count MEMBER sampleCount)
    Q_CLASSINFO("table", "Samples")
    Q_CLASSINFO("fields", "secretId(int, required)")
    S_DATA(SampleData)

public:
    SampleData() { init(); }

    SampleData(const SampleData &other) : Systemus::Data(other, false),
        name(other.name), description(other.description), sampleCount(other.sampleCount)
    {}

    void initSample()
    { }

    QString name;
    QString description;
    int sampleCount;
};

Q_DECLARE_METATYPE(SampleData)

class DataInfoTest : public testing::Test
{
protected:
    void SetUp() override
    { sample.initSample(); }

    SampleData sample;
};

inline QMetaProperty property(const char *name, const QMetaObject *object)
{ return object->property(object->indexOfProperty(name)); }

inline bool operator==(const QMetaProperty &p1, const QMetaProperty &p2)
{ return p1.name() == p2.name() && p1.enclosingMetaObject() == p2.enclosingMetaObject(); }

#endif // DATAINFOTEST_H
