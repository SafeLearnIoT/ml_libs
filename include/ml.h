#pragma once
#include "ArduinoJson.h"
#include "utils.h"
#include "rtpnn.h"
#include "LinearRegression.h"
#include "LogisticRegression.h"
#include "NeuralNetwork.h"
#include <array>
#include <vector>
#include <Arduino.h>
#include <numeric>

#define NumberOf(arg) ((unsigned int)(sizeof(arg) / sizeof(arg[0]))) // calculates the number of layers (in this case 3)

#define SELU

#define DEBUG
#define TRY_PREDICT

const unsigned int layers[] = {1, 3, 1}; // 3 layers (1st)layer with 1 input neurons (2nd)layer with 3 hidden neurons and (3rd)layer with 1 output neuron

class ML
{
protected:
    String m_name;
    MLAlgo m_algo;
    SensorType m_sensor_type;
    RTPNN::SDP *m_rtpnn;
    LogisticRegression *m_log_reg;
    LinearRegression *m_lin_reg;
    NeuralNetwork *m_mlp;

public:
    explicit ML(SensorType sensor_type, MLAlgo algo, String name) : m_sensor_type(sensor_type), m_algo(algo), m_name(name)
    {
        switch (m_algo)
        {
        case MLAlgo::LinReg:
            m_lin_reg = new LinearRegression();
        case MLAlgo::LogReg:
            m_log_reg = new LogisticRegression();
        case MLAlgo::rTPNN:
            m_rtpnn = new RTPNN::SDP(sensor_type);
        case MLAlgo::MLP:
            m_mlp = new NeuralNetwork(layers, NumberOf(layers));
        case MLAlgo::None:
        default:
            break;
        }
    }

    void perform(tm time, float value, JsonObject &data)
    {
        double values[2];
        auto time_conv = time.tm_hour * 60 + time.tm_min;

#ifdef DEBUG
        Serial.println(" ===== Perform enter ===== ");
        Serial.print("Time value: ");
        Serial.println(time_conv);
        Serial.print("Sensor value: ");
        Serial.println(value);
#endif

        if (time_conv == 0)
        {
            Serial.println("Time equal to 0, might break calculations");
        }

        // Log times of learning and prediction
        switch (m_algo)
        {
        case MLAlgo::LinReg:
        {
            m_lin_reg->learn(static_cast<double>(time_conv), static_cast<double>(value));
            m_lin_reg->parameters(values);

            data[m_name]["a"] = values[0];
            data[m_name]["b"] = values[1];

            break;
        }
        case MLAlgo::LogReg:
        {
            auto time_n = normalize(static_cast<double>(time_conv), 0, 1440);
            auto value_n = 0 ? value <= 18 : 1;

            m_log_reg->addSample(time_n, value_n);
            m_log_reg->train();

            data[m_name]["weight"] = m_log_reg->getWeight();
            data[m_name]["bias"] = m_log_reg->getBias();

            break;
        }
        case MLAlgo::MLP:
        {
            const float input[1] = {static_cast<const float>(time_conv)};
            const float expectedOutput[1] = {static_cast<const float>(value)};
            m_mlp->FeedForward(input);
            m_mlp->BackProp(expectedOutput);

            data[m_name]["nn_data"] = m_mlp->save_to_blob();
            break;
        }
        case MLAlgo::rTPNN:
        {
            break;
        }
        case MLAlgo::None:
        {
            break;
        }
        default:
        {
            break;
        }
        }

#ifdef DEBUG
        Serial.print("ML Output: ");
        serializeJson(data[m_name], Serial);
        Serial.println();

#ifdef TRY_PREDICT
        if (m_algo == MLAlgo::LinReg)
        {
            Serial.print("[Linear Regression] Experimenal prediction for current time: ");
            Serial.println(m_lin_reg->calculate(time_conv));
        }
        else if (m_algo == MLAlgo::LogReg)
        {
            Serial.print("[Logistic Regression] Experimenal prediction for current time: ");
            Serial.println(m_log_reg->predict_single(normalize(static_cast<double>(time_conv), 0, 1440)));
        }
#endif
        Serial.println(" ===== Perform exit ===== ");
#endif
    };
};