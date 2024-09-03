#pragma once

#include <vector>
#include <Arduino.h>
#include <ml_common.h>

namespace RTPNN
{
    class SDP : public ML
    {
        std::vector<double> m_levels{0.5};
        std::vector<double> m_trends{0.5};

        float m_trend_param_1 = 0.5;
        float m_trend_param_2 = 0.5;

        float m_level_param_1 = 0.5;
        float m_level_param_2 = 0.5;

        void calculate_trend(float &value);
        void calculate_level(float &value);

    public:
        using ML::ML;
        virtual JsonObject perform(tm time, float value) override;
        virtual void train() override {};
        virtual float predict(float x) override { return 0.0f; };
    };
}