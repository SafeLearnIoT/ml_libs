#include "rtpnn.h"

namespace rTPNN
{
    template class SDP<float>;
    template class SDP<uint16_t>;
    template class SDP<int>;

    template <typename T>
    inline void SDP<T>::calculate_trend(T &value)
    {
        trends.push_back((trend_param_1 * (normalize(value) - normalize(values.back()))) + (trend_param_2 * trends.back()));
    }

    template <typename T>
    inline void SDP<T>::calculate_level(T &value)
    {
        levels.push_back((level_param_1 * normalize(value)) + (level_param_2 * levels.back()));
    }

    template <typename T>
    float SDP<T>::normalize(T &value)
    {
        return (value - min) / (max - min);
    }

    template <typename T>
    std::pair<float, float> SDP<T>::execute_sdp(T value)
    {
        // First execution - Skip calculations, just add value to the Vector<T>
        if (!values.empty())
        {
            calculate_level(value);
            calculate_trend(value);
        }
        values.push_back(value);

        return std::make_pair(trends.back(), levels.back());
    }
}