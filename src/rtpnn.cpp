#include "rtpnn.h"
#include <numeric>

namespace RTPNN
{
    void SDP::calculate_trend(double &value)
    {
        m_trends.push_back((m_trend_param_1 * (value - m_values.back())) + (m_trend_param_2 * m_trends.back()));
    }

    void SDP::calculate_level(double &value)
    {
        m_levels.push_back((m_level_param_1 * value) + (m_level_param_2 * m_levels.back()));
    }

    void SDP::perform(double& value, JsonObject& obj)
    {
        auto val_norm = normalize(value, m_min, m_max);
        // First execution - Skip calculations, just add value to the Vector<T>
        if (!m_values.empty())
        {
            calculate_level(val_norm);
            calculate_trend(val_norm);
            obj["trend"] = m_trends.back();
            obj["level"] = m_levels.back();
            serializeJson(obj, Serial);
            Serial.println();
        }
        m_values.push_back(val_norm);

        m_predictions.push_back(predict(val_norm));

        if(!validate_size()){
            Serial.println("Vector sizes are not equal");
        }
        
        if(m_values.size() == 16){
            train();
        }

        Serial.println("[rTPNN] Values count: " + String(m_values.size()));
    }

    double SDP::predict(double &value)
    {
        return phi(m_weights[0] * m_trends.back() + m_weights[1] * m_levels.back() + m_weights[2] * value + m_weights[3]);
    }

    double SDP::mse()
    {
        auto pred_val_diff = std::accumulate(m_values.begin(), m_values.end(), 0.0) - std::accumulate(m_predictions.begin(), m_predictions.end(), 0.0);
        return (1 / m_values.size()) * (pred_val_diff * pred_val_diff);
    }

    double SDP::phi(double z)
    {
        return 1 / (1 + exp(-z));
    }

    double SDP::Dphi(double z)
    {
        return z * (1 - z);
    }


    double SDP::bptt(double weight, Arg arg)
    {
        double sum = 0.0;
        
        int N = static_cast<int>(m_values.size()); 
        for (int i = 0; i < N; ++i)
        {
            auto calc = part_sum(i); // dL/dx
            
            switch (arg)
            {
                case Trends:
                    calc *= m_trends[i];
                    break;
                case Levels:
                    calc *= m_levels[i];
                    break;
                case Values:
                    calc *= m_values[i];
                    break;
                case None:
                    break;
            }
            
            sum += calc;
        }

        return weight - (m_lr * 2 / N) * sum;
    }

    double SDP::part_sum(int i)
    {
        return (m_values[i] - m_predictions[i]) * -1.0 * Dphi(m_predictions[i]);
    }

    void SDP::update_trend_params()
    {
        auto sum = 0.0;
        for(int i = 3; i < m_values.size(); ++i)
        {
            sum += part_sum(i) * m_weights[0] // dL/d^xk * d^xk/dt^k
            * ( 
                (m_values[i] - m_values[i - 1]) // dt^k/du^1
                + 
                (m_trend_param_2 * (m_values[i-1] - m_values[i - 2]) ) // dt^k/dt^k-1 * dl^k-1/du^1
                +
                ( m_trend_param_2 * m_trend_param_2 * (m_values[i - 2] - m_values[i - 3]) ) //dt^k/dt^k-1 * dt^k-1/dt^k-2 * dt^k-2/du^1
            );
        }

        m_trend_param_1 = m_trend_param_1 - (m_lr * 2 / m_values.size()) * sum;

        sum = 0.0;
        for(int i = 3; i < m_values.size(); ++i)
        {
            sum += part_sum(i) * m_weights[0] // dL/d^xk * d^xk/dt^k
            * ( 
                (m_values[i] - m_values[i - 1]) // dt^k/du^2
                + 
                (m_trend_param_2 * m_trends[i-2] ) // dt^k/dt^k-1 * dl^k-1/du^2
                +
                ( m_trend_param_2 * m_trend_param_2 * m_trends[i - 3]) //dt^k/dt^k-1 * dt^k-1/dt^k-2 * dt^k-2/du^2
            );
        }
        
        m_trend_param_2 = m_trend_param_2 - (m_lr * 2 / m_values.size()) * sum;
    }

    void SDP::update_level_params()
    {
        auto sum = 0.0;
        for(int i = 3; i < m_values.size(); ++i)
        {
            sum += part_sum(i) * m_weights[1]  // dL/d^xk * d^xk/dl^k
            *  ( 
                (m_values[i] - m_values[i - 1]) // dl^k/dv^1
                +
                (m_level_param_2 * m_values[i-1]) // dl^k/dl^k-1 * dl^k-1/dv^1
                +
                ( m_level_param_2 * m_level_param_2 * m_values[i - 2]) //dl^k/dl^k-1 * dl^k-1/dl^k-2 * dl^k-2/dv^1
            );
        }
        m_level_param_1 = m_level_param_1 - (m_lr * 2 / m_values.size()) * sum;

        sum = 0.0;
        for(int i = 3; i < m_values.size(); ++i)
        {
            sum += part_sum(i) * m_weights[1]  // dL/d^xk * d^xk/dl^k
            *  ( 
                (m_values[i] - m_values[i - 1]) // dl^k/dv^2
                +
                (m_level_param_2 * m_levels[i-2]) // dl^k/dl^k-1 * dl^k-1/dv^2
                +
                ( m_level_param_2 * m_level_param_2 * m_levels[i - 3]) //dl^k/dl^k-1 * dl^k-1/dl^k-2 * dl^k-2/dv^2
            );
        }
        m_level_param_2 = m_level_param_2 - (m_lr * 2 / m_values.size()) * sum;
    }

    void SDP::get_weights(JsonDocument& doc, String key)
    {
        JsonArray array = doc[key].to<JsonArray>();

        array.add(m_weights[0]);
        array.add(m_weights[1]);
        array.add(m_weights[2]);
        array.add(m_weights[3]);

        
    }

    void SDP::set_weights(std::array<double, 4>& weights)
    {
        m_weights = weights;
        Serial.println("[rTPNN] Weights set W1: " + String(m_weights[0]) + " W2: " + String(m_weights[1]) + " W3: " + String(m_weights[2]) + " W4: " + String(m_weights[3]));
    }

    bool SDP::validate_size()
    {
        if(m_values.size() != m_levels.size() || m_values.size() != m_trends.size() || m_values.size() != m_predictions.size())
        {
            return false;
        }
        return true;
    }

    void SDP::train(){
        int epochs = 3;
        for(int i = 0; i < epochs; ++i){
            // Update weights
            m_weights[0] = bptt(m_weights[0], Arg::Trends);
            m_weights[1] = bptt(m_weights[1], Arg::Levels);
            m_weights[2] = bptt(m_weights[2], Arg::Values);
            m_weights[3] = bptt(m_weights[3], Arg::None);
        
            // Update trend and level parameters
            update_trend_params();
            update_level_params();
        }

        // After training, clear the vectors but leave last value
        m_values.erase(m_values.begin(), m_values.end() - 1);
        m_levels.erase(m_levels.begin(), m_levels.end() - 1);
        m_trends.erase(m_trends.begin(), m_trends.end() - 1);
        m_predictions.erase(m_predictions.begin(), m_predictions.end() - 1);
    }
}