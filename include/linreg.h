#include <array>
#include <vector>
#include <Arduino.h>

template <typename T>
class LinReg
{
    std::vector<unsigned long> timeSeries; // X
    std::vector<T> values;                 // y
    std::vector<float> weights = {0.0f};   // weights

    void train(float alpha, int epochs)
    {
        weights = {0.0f};
        for (int epoch = 0; epoch < epochs; ++epoch)
        {
            std::vector<float> gradients = {0.0f};

            for (int i = 0; i < timeSeries.size(); ++i)
            {
                float pred = weights[0] + weights[1] * timeSeries[i];
                float err = pred - values[i];

                gradients[0] += err;
                gradients[1] += err * timeSeries[i];
            }

            weights[0] -= alpha * gradients[0];
            weights[1] -= alpha * gradients[1];
        }
    }

    T predict(T X)
    {
        return weights[0] + weights[1] * X;
    }

    void perform(std::vector<T> input)
    {
        float alpha = 0.01f;
        int epochs = 1000;

        trainLinearRegression(alpha, epochs);

        for (const auto &x : input)
        {
            std::cout << "x: " << x << " | pred: " << predict(x) << std::endl;
        }
    }
};