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
    explicit ML(SensorType sensor_type, String name) : m_sensor_type(sensor_type), m_name(name)
    {
        m_rtpnn = new RTPNN::SDP(sensor_type);
    }

    void perform(double value)
    {
        m_rtpnn->perform(value);
    };

    JsonDocument get_weights()
    {
        JsonDocument doc;
        JsonArray array = doc.to<JsonArray>();
        auto weights = m_rtpnn->get_weights();

        array.add(weights[0]);
        array.add(weights[1]);
        array.add(weights[2]);
        array.add(weights[3]);

        doc["weights"] = array;
        return doc;
    }
};