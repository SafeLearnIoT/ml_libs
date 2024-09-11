#pragma once
#include <Arduino.h>
#include <map>
#include <ArduinoJson.h>

enum MLAlgo
{
    None,
    rTPNN,
    LinReg,
    LogReg
};

enum SensorType
{
    Temperature = 0, // Min: -40     Max: 85       (Celcius degrees)
    Humidity = 1,    // Min: 0       Max: 100      (Percentage)
    IAQ = 2,         // Min: 0       Max: 500      (Quality score)
    Pressure = 3,    // Min: 30000   Max: 110000   (Pa)
    Light = 4,       // Min: 21      Max: 3256     (Analog read)
    Window = 5,      // Min: 0       Max: 1        (Bool)
    Blinds = 6,      // Min: 0       Max: 1        (Bool)
};

struct SensorConfig
{
    double minValue;
    double maxValue;
};

inline std::map<SensorType, SensorConfig> sensorConfigs = {
    {Temperature, {-40, 85}},
    {Humidity, {0, 100}},
    {IAQ, {0, 500}},
    {Pressure, {30000, 110000}},
    {Light, {21, 3256}},
    {Window, {0, 1}},
    {Blinds, {0, 1}},
};

inline float normalize(float value, float min, float max)
{
    return static_cast<float>(value - min) / static_cast<float>(max - min);
}

inline double sigmoid(double z){
    return 1/(1+exp(-z));
}

inline double dot(const std::vector<unsigned long>& a, const std::vector<double>& b) {
    // Compute the dot product
    int result = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}