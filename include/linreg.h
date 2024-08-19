#pragma once
#include "ml_common.h"

namespace Regression
{
    class Linear : public ML
    {
        float m_b0 = 0.0f;
        float m_b1 = 0.0f;

    public:
        using ML::ML;

        void train() override;
        JsonObject perform(tm time, float value) override;
        float predict(float x) override;
    };
}