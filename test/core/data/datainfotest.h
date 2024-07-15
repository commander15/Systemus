#ifndef DATAINFOTEST_H
#define DATAINFOTEST_H

#include <gtest/gtest.h>

#include <SystemusCore/data.h>

class SampleData : public Systemus::Data
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER name USER true)
    Q_PROPERTY(QString description MEMBER description)
    Q_PROPERTY(int count MEMBER sampleCount)
    Q_CLASSINFO("table", "Samples")
    Q_CLASSINFO("fields", "secret_id(int, required)")
    S_DATA(SampleData)

public:
    SampleData() { init(); }
    void initSample()
    { }

    QString name;
    QString description;
    int sampleCount;
};

class DataInfoTest : public testing::Test
{
protected:
    void SetUp() override
    { sample.initSample(); }

    SampleData sample;
};

#endif // DATAINFOTEST_H
