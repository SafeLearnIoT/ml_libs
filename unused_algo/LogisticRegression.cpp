#include "Arduino.h"
#include "LogisticRegression.h"

LogisticRegression::LogisticRegression(double lr, int num_epochs)
    : learning_rate(lr), epochs(num_epochs), weight(0.0), bias(0.0) {}

double LogisticRegression::sigmoid(double z) const
{
    return 1.0 / (1.0 + std::exp(-z));
}

double LogisticRegression::predict_single(double x) const
{
    return sigmoid(weight * x + bias);
}

double LogisticRegression::compute_loss() const
{
    double loss = 0.0;
    for (size_t i = 0; i < Y.size(); ++i)
    {
        double y_pred = predict_single(X[i]);
        y_pred = std::min(std::max(y_pred, 1e-10), 1 - 1e-10); // Clip between 1e-10 and 1-1e-10
        loss += -Y[i] * std::log(y_pred) - (1 - Y[i]) * std::log(1 - y_pred);
    }
    return loss / Y.size();
}

void LogisticRegression::gradient_descent_step()
{
    double weight_gradient = 0.0;
    double bias_gradient = 0.0;

    for (size_t i = 0; i < X.size(); ++i)
    {
        double prediction = predict_single(X[i]);
        weight_gradient += (prediction - Y[i]) * X[i];
        bias_gradient += (prediction - Y[i]);
    }

    // Update weight and bias
    weight -= learning_rate * weight_gradient / X.size();
    bias -= learning_rate * bias_gradient / X.size();
}

void LogisticRegression::train()
{
    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        gradient_descent_step();

        // Print loss every 100 epochs
        if (epoch % 100 == 0)
        {
            double loss = compute_loss();
            Serial.print("Epoch ");
            Serial.print(epoch);
            Serial.print(", Loss: ");
            Serial.println(loss);
        }
    }
}

std::vector<double> LogisticRegression::predict() const
{
    std::vector<double> predictions;
    for (const double &x : X)
    {
        predictions.push_back(predict_single(x));
    }
    return predictions;
}

// Get predictions (0 or 1) based on a threshold of 0.5
std::vector<int> LogisticRegression::predict_labels() const
{
    std::vector<int> labels;
    for (const double &x : X)
    {
        double prob = predict_single(x);
        labels.push_back(prob >= 0.5 ? 1 : 0);
    }
    return labels;
}

// Function to set a new dataset
void LogisticRegression::set_data(const std::vector<double> &X_data, const std::vector<int> &Y_data)
{
    X = X_data;
    Y = Y_data;
}

void LogisticRegression::addSample(const double X_val, const int Y_val)
{
    X.push_back(X_val);
    Y.push_back(Y_val);
}

double LogisticRegression::getWeight()
{
    return weight;
}

double LogisticRegression::getBias()
{
    return bias;
}
