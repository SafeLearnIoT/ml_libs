#include "logreg.h"

namespace Regression
{
    float Logistic::getElementLogisticCost(unsigned long &x, float &y, float &a, float &b)
    {
        float p = 1 / (1 + exp(-(a * x + b)));
        if (y == 1)
        {
            return -log(p);
        }
        else
        {
            return -log(1 - p);
        }
    }

    float Logistic::getLogisticCost(float a, float b, float &da, float &db)
    {
        int n = static_cast<int>(m_time.size());

        float cost = 0;
        da = 0;
        db = 0;

        for (int i = 0; i < n; i++)
        {
            cost += getElementLogisticCost(m_time[i], m_values[i], a, b);
            float eaxb = exp(a * m_time[i] + b);

            if (m_values[i] == 1)
            {
                da += -m_time[i] / (1 + eaxb);
                db += -1 / (1 + eaxb);
            }

            else
            {
                da += m_time[i] * eaxb / (1 + eaxb);
                db += eaxb / (1 + eaxb);
            }
        }

        cost /= n;
        da /= n;
        db /= n;

        return cost;
    }

    void Logistic::train()
    {
        float lrate = 0.0005;
        float threshold = 0.001;
        int iter = 0;

        while (true)
        {
            float da = 0;
            float db = 0;
            double cost = getLogisticCost(m_slope, m_intercept, da, db);

            if (iter % 1000 == 0)
            {
                Serial.println("Iter : " + String(iter) + " cost = " + String(cost) + " da = " + String(da) + " db = " + String(db));
            }

            iter++;

            if (abs(da) < threshold && abs(db) < threshold)
            {
                Serial.println("p = 1 / (1 + exp(-(" + String(m_slope) + " * x + " + String(m_intercept) + ")))");
                break;
            }
            m_slope -= lrate * da;
            m_intercept -= lrate * db;
        }
    }

    JsonObject Logistic::perform(tm time, float value)
    {
        m_time.push_back(time.tm_hour * 60 + time.tm_min);
        m_values.push_back(value);

        train();
        JsonDocument doc;
        JsonObject object = doc.to<JsonObject>();

        object["slope"] = m_slope;
        object["intercept"] = m_intercept;
        return object;
    }
}
