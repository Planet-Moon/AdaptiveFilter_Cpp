#include "WhiteNoise.h"
#include <chrono>

WhiteNoise::WhiteNoise(double mean, double stdev_):
    mean(mean), stddev(stdev_)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator = std::default_random_engine(seed);
    distribution = std::normal_distribution<double>(mean, stddev);
}

double WhiteNoise::generate()
{
    return distribution(generator);
}
