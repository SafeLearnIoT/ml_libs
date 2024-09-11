#pragma once
#include <vector>
#include <Arduino.h>
#include <utils.h>

namespace RTPNN
{
    class SDP
    {
        std::vector<int> m_time; // Minutes since midnight
        std::vector<double> m_values;      // values

        std::vector<double> m_levels{0.5};
        std::vector<double> m_trends{0.5};

        float m_trend_param_1 = 0.5;
        float m_trend_param_2 = 0.5;

        float m_level_param_1 = 0.5;
        float m_level_param_2 = 0.5;

        float m_min = 0.0f;
        float m_max = 0.0f;

        void calculate_trend(float &value);
        void calculate_level(float &value);

    public:
        SDP(SensorType sensor_type){
            m_min = sensorConfigs.at(sensor_type).minValue;
            m_max = sensorConfigs.at(sensor_type).maxValue;
        };
        JsonObject perform(float value);
        void train() {};
        float predict(float x) { return 0.0f; };
    };
}