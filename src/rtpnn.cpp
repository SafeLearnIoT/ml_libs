#include "rtpnn.h"

namespace RTPNN
{
    void SDP::calculate_trend(float &value)
    {
        m_trends.push_back((m_trend_param_1 * (normalize(value, m_min, m_max) - normalize(m_values.back(), m_min, m_max))) + (m_trend_param_2 * m_trends.back()));
    }

    void SDP::calculate_level(float &value)
    {
        m_levels.push_back((m_level_param_1 * normalize(value, m_min, m_max)) + (m_level_param_2 * m_levels.back()));
    }

    JsonObject SDP::perform(float value)
    {
        // First execution - Skip calculations, just add value to the Vector<T>
        if (!m_values.empty())
        {
            calculate_level(value);
            calculate_trend(value);
        }
        m_values.push_back(value);

        JsonDocument doc;
        JsonObject object = doc.to<JsonObject>();
        object["trend"] = m_trends.back();
        object["level"] = m_levels.back();
        return object;
    }
}