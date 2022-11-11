#include "gtest/gtest.h"
#include <Matrix.h>
#include <functional>

// https://www.kalmanfilter.net/kalmanmulti.html

TEST(Kalman, multidim){
    const double delta_t = 5;
    const double delta_t_2 = 0.5 * std::pow(delta_t, 2);

    // state transition matrix
    Mat F = {
        {1,0,0,delta_t,0,0,delta_t_2,0,0},
        {0,1,0,0,delta_t,0,0,delta_t_2,0},
        {0,0,1,0,0,delta_t,0,0,delta_t_2},
        {0,0,0,1,0,0,delta_t,0,0},
        {0,0,0,0,1,0,0,delta_t,0},
        {0,0,0,0,0,1,0,0,delta_t},
        {0,0,0,0,0,0,1,0,0},
        {0,0,0,0,0,0,0,1,0},
        {0,0,0,0,0,0,0,0,1}
    };

    Vec systemState{
        0,0,0,0,0,0,0,0,0
    };

    const std::function<Vec(const Vec&, const Vec&)> state_extrapolation = \
        [F]
        (const Vec& systemState, const Vec& measurement)->Vec
    {
        Vec result;
        result = F * systemState;
        return result;
    };
}

TEST(Kalman, fallingObject){
    const double delta_t = 5;
    const double delta_t_2 = 0.5 * std::pow(delta_t, 2);

    // state transition matrix
    Mat F = {
        {1, delta_t},
        {0, 1}
    };

    // control matrix G
    Mat G = {
        {delta_t_2},
        {delta_t}
    };

    Vec systemState{
        0, // altitude
        0  // velocity
    };

    // input variable
    Vec u = {
        9.81 // gravity constant
    };

    const std::function<Vec(const Vec&)> state_extrapolation = \
        [&, F, G, u]
        (const Vec& systemState)->Vec
    {
        Vec result;
        const auto one = F * systemState;
        const auto two = G * u;
        result = one + two;
        return result;
    };
}
