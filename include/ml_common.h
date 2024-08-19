#pragma once
#include "ArduinoJson.h"
#include "utils.h"
#include <array>
#include <vector>
#include <Arduino.h>
#include <numeric>

class ML
{
public:
    explicit ML(SensorType sensor_type) : m_type(sensor_type)
    {
        m_min = sensorConfigs.at(sensor_type).minValue;
        m_max = sensorConfigs.at(sensor_type).maxValue;
    }

    virtual JsonObject perform(tm time, float value) = 0;
    virtual float predict(float x) = 0;
    virtual void train() = 0;

protected:
    std::vector<unsigned long> m_time; // Minutes since midnight
    std::vector<float> m_values;       // values
    SensorType m_type;
    float m_min = 0.0f;
    float m_max = 0.0f;
};