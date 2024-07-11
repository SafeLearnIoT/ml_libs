#ifndef RTPNN_H

#define RTPNN_H

#include <vector>
#include <Arduino.h>

namespace rTPNN
{
    enum SDPType
    {
        Temperature = 0, // Min: -40     Max: 85       (Celcius degrees)
        Humidity = 1,    // Min: 0       Max: 100      (Percentage)
        IAQ = 2,         // Min: 0       Max: 500      (Quality score)
        Pressure = 3,    // Min: 30000   Max: 110000   (Pa)
        Light = 4,       // Min: 21      Max: 3256     (Analog read)
        Window = 5,      // Min: 0       Max: 1        (Bool)
        Blinds = 6,      // Min: 0       Max: 1        (Bool)
    };

    template <typename T>
    class SDP
    {
        std::vector<T> values;
        std::vector<float> levels{0.5};
        std::vector<float> trends{0.5};

        float trend_param_1 = 0.5;
        float trend_param_2 = 0.5;

        float level_param_1 = 0.5;
        float level_param_2 = 0.5;

        SDPType type;
        float min;
        float max;

        void calculate_trend(T &value);
        void calculate_level(T &value);
        float normalize(T &value);

    public:
        SDP(SDPType sdp_type) : type(sdp_type)
        {
            switch (type)
            {
            case SDPType::Temperature:
                min = -40.0f;
                max = 85.0f;
                break;
            case SDPType::Humidity:
                min = 0.0f;
                max = 100.0f;
                break;
            case SDPType::IAQ:
                min = 0.0f;
                max = 500.0f;
                break;
            case SDPType::Pressure:
                min = 30000.0f;
                max = 110000.0f;
                break;
            case SDPType::Light:
                min = 21.0f;
                max = 3256.0f;
                break;
            case SDPType::Window:
                min = 0.0f;
                max = 1.0f;
                break;
            case SDPType::Blinds:
                min = 0.0f;
                max = 1.0f;
                break;
            default:
                break;
            }
        };

        std::pair<float, float> execute_sdp(T value);
    };
}

#endif