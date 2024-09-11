#pragma once
#include "ArduinoJson.h"
#include "utils.h"
#include "rtpnn.h"
#include "LinearRegression.h"
#include "LogisticRegression.h"
#include <array>
#include <vector>
#include <Arduino.h>
#include <numeric>

#define DEBUG
#define TRY_PREDICT

class ML
{
protected:
    MLAlgo m_algo;
    RTPNN::SDP *m_rtpnn;
    LogisticRegression *m_log_reg;
    LinearRegression *m_lin_reg;
public:
    explicit ML(SensorType sensor_type, MLAlgo algo) : m_algo(algo)
    {
        switch (m_algo){
            case MLAlgo::LinReg:
                m_lin_reg = new LinearRegression();
            case MLAlgo::LogReg:
                m_log_reg = new LogisticRegression();
            case MLAlgo::rTPNN:
                m_rtpnn = new RTPNN::SDP(sensor_type);
            case MLAlgo::None:
            default:
                break;
        }

    }

    JsonObject perform(tm time, float value){
        JsonDocument doc;
        JsonObject object = doc.to<JsonObject>();
        double values[2];
        auto time_conv = time.tm_hour * 60 + time.tm_min;

#ifdef DEBUG        
        Serial.println(" ===== Perform enter ===== ");
        Serial.print("Time value: ");
        Serial.println(time_conv);
        Serial.print("Sensor value: ");
        Serial.println(value);
#endif

        if(time_conv == 0){
            Serial.println("Time equal to 0, might break calculations");
            return object;
        }

        switch (m_algo){
            case MLAlgo::LinReg:
                m_lin_reg->learn(static_cast<double>(time_conv), static_cast<double>(value));
                m_lin_reg->parameters(values);
                
                object["a"] = values[0];
                object["b"] = values[1];
                
                break;
            case MLAlgo::LogReg:
                m_log_reg->learn(static_cast<double>(time_conv), static_cast<double>(value));
                m_log_reg->parameters(normalize(values));
                
                object["a"] = values[0];
                object["b"] = values[1];
                
                break;
            case MLAlgo::rTPNN:
                object = m_rtpnn->perform(value);
                break;
            case MLAlgo::None:
            default:
                break;
        }

#ifdef DEBUG
        Serial.print("ML Output: ");
        serializeJson(object, Serial);
        Serial.println();
        
#ifdef TRY_PREDICT
        if (m_algo == MLAlgo::LinReg) {
        Serial.print("[Linear Regression] Experimenal prediction for current time: ");
        Serial.println(m_lin_reg->calculate(time_conv));
        }
        else if (m_algo == MLAlgo::LogReg){
        Serial.print("[Logistic Regression] Experimenal prediction for current time: ");
        Serial.println(m_log_reg->calculate(time_conv));
        }
#endif
        Serial.println(" ===== Perform exit ===== ");
#endif
        return object;
    };
};