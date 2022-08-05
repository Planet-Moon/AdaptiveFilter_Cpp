#include "WhiteNoise.h"
#include <chrono>

WhiteNoise::WhiteNoise(double mean, double variance):
    mean(mean), stddev(variance)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator = std::default_random_engine(seed);
    distribution = std::normal_distribution<double>(0.0,1.0);
}

double WhiteNoise::generate()
{
    return distribution(generator);
}
