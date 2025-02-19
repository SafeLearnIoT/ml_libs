#pragma once
#include "ArduinoJson.h"
#include "utils.h"
#include "rtpnn.h"
#include <array>
#include <vector>
#include <Arduino.h>
#include <numeric>

class ML
{
protected:
    String m_name;
    SensorType m_sensor_type;
    RTPNN::SDP *m_rtpnn;

public:
    ML(SensorType sensor_type, String name) : m_sensor_type(sensor_type), m_name(name)
    {
        m_rtpnn = new RTPNN::SDP(sensor_type);
    }

    void perform(double value, JsonObject &obj)
    {
        m_rtpnn->perform(value, obj);
    };

    void get_weights(JsonDocument& doc)
    {
        m_rtpnn->get_weights(doc, m_name);
    }

    void set_weights(std::array<double, 4>& weights)
    {
        m_rtpnn->set_weights(weights);
    }
};