#ifndef LogisticRegression_h
#define LogisticRegression_h

#include "Arduino.h"
#include <vector>

class LogisticRegression
{
    std::vector<double> X{};
    std::vector<int> Y{};
    double weight;
    double bias;
    double learning_rate;
    int epochs;

    double sigmoid(double z) const;
    double compute_loss() const;
    void gradient_descent_step();

public:
    LogisticRegression(double lr = 0.01, int num_epochs = 1000);
    void train();
    double predict_single(double x) const;
    std::vector<double> predict() const;
    std::vector<int> predict_labels() const;
    void set_data(const std::vector<double> &X_data, const std::vector<int> &Y_data);
    void addSample(const double X_val, const int Y_val);
    double getWeight();
    double getBias();
};

#endif