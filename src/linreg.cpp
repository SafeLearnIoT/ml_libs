#include "linreg.h"

namespace Regression
{
    void Linear::train()
    {
        auto n = m_time.size();

        auto avg_time = std::accumulate(m_time.begin(), m_time.end(), 0.0f) / n;
        auto avg_value = std::accumulate(m_values.begin(), m_values.end(), 0.0f) / n;

        float sum_x_x = 0.0f;
        float sum_x_y = 0.0f;

        for (size_t i = 0; i < n; ++i)
        {
            sum_x_x += m_time.at(i) * m_time.at(i);
            sum_x_y += m_time.at(i) * m_values.at(i);
        }

        auto cross_deviation = sum_x_y - (n * avg_time * avg_value);
        auto deviation = sum_x_x - (n * avg_time * avg_time);

        m_b1 = cross_deviation / deviation;
        m_b0 = avg_value - m_b1 * avg_time;
    }

    float Linear::predict(float X)
    {
        return m_b0 + m_b1 * X;
    }

    JsonObject Linear::perform(tm time, float value)
    {
        m_time.push_back(time.tm_hour * 60 + time.tm_min);
        m_values.push_back(value);

        train();
        JsonDocument doc;
        JsonObject object = doc.to<JsonObject>();

        object["m_b0"] = m_b0;
        object["m_b1"] = m_b1;
        return object;
    }

}