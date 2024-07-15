#ifndef DATATEST_H
#define DATATEST_H

#include <gtest/gtest.h>

#include <SystemusCore/datarelation.h>

#include "datainfotest.h"

class SensitiveSample : public SampleData
{
    Q_GADGET
    Q_PROPERTY(int riskLevel MEMBER riskLevel)
    Q_CLASSINFO("fields", "ref_sample(int)")
    S_DATA(SensitiveSample)

public:
    SensitiveSample() { init(); }
    SensitiveSample(const SensitiveSample &other) = default;

    int riskLevel = 0;

    Systemus::OneToOneRelation<SampleData> ref;
};

class DataTest : public testing::Test
{
public:
    SampleData sample;

    // Test interface
protected:
    void SetUp() override
    { sample.initSample(); }
};

#endif // DATATEST_H
