#pragma once

#include "ml_common.h"

namespace Regression
{
    class Logistic : public ML
    {
        float m_slope = 1.0f;
        float m_intercept = 0.0f;

    public:
        using ML::ML;

        float getElementLogisticCost(unsigned long &x, float &y, float &a, float &b);
        float getLogisticCost(float a, float b, float &da, float &db);

        void train() override;
        JsonObject perform(tm time, float value) override;
        float predict(float x) override;
    };
}