#pragma once
#include <random>

class WhiteNoise{
public:
    WhiteNoise(double mean, double stddev);
    double generate();
protected:
private:
    double mean = 0.0;
    double stddev = 0.1;
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
};
