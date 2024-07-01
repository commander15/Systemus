#ifndef DATAINFOTEST_H
#define DATAINFOTEST_H

#include <gtest/gtest.h>

#include <SystemusCore/data.h>

class SampleData : public Systemus::Data
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(int count MEMBER count)
    Q_CLASSINFO("table", "Samples")
    Q_CLASSINFO("fields", "secret_id(int, required)")
    S_DATA(SampleData)

    QString name;
    QString description;
    int count;
};

class DataInfoTest : public testing::Test
{
protected:
    SampleData sample;
};

#endif // DATAINFOTEST_H
