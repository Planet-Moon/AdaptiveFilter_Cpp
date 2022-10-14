#include "gtest/gtest.h"
#include <functional>
#include <Matrix.h>

TEST(Kalman, BUILD){
    EXPECT_TRUE(true);
}

TEST(Kalman, there){
    const double delta_t = 0.5;
    // system state
    Vec x = {0, 0};
    // state-transition model
    Mat F = {
        {1, delta_t},
        {0, 1}
    };
    // observation model
    Mat H = {
        {0, 0},
        {0, 0}
    };
    // covariance of process noise
    Mat Q;
    // covariance of observation noise
    Mat R;
    // control input model
    Mat B = {
        {0, 0},
        {0, 0}
    };
    // control vector
    Vec u = {0, 0};
    Mat G{
        {0.5*std::pow(delta_t,2)},
        {delta_t}
    };

    // process noise
    double w{};

    Vec x_assumed;
    for(int k = 1; k<10; ++k){
        x_assumed = F*x + B*u + w;
    }

    // observation noise
    Vec v = {0, 0};
    // observation
    Vec z = H*x + v;
}

double calculate_gain(const double n){
    return 1/n;
}

double calculate_current_estimate(const double& old_estimate, const double& gain, const double& measurement){
    return old_estimate + gain* (measurement - old_estimate);
}

TEST(alpha_beta_gamma, GoldExample){
    // https://www.kalmanfilter.net/alphabeta.html

    // iteration counter
    int n = 1;

    // initial guess of the gold weight
    double x = 1000; // g

    // dynamic model of the system is static
    // -> next x is the same value

    // first iteration
    n = 1;
    double z_1 = 1030; // g


    // calculate gain ( here: 1/n )
    double alpha = calculate_gain(n);

    // current estimate
    x = calculate_current_estimate(x, alpha, z_1);

    // second iteration
    n = 2;
    double z_2 = 989; // g

    alpha = calculate_gain(n);

    x = calculate_current_estimate(x, alpha, z_2);

    // third iteration
    n = 3;
    double z_3 = 1017;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_3);

    // fourth iteration
    n = 4;
    double z_4 = 1009;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_4);

    // fourth iteration
    n = 5;
    double z_5 = 1013;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_5);

    // sixth iteration
    n = 6;
    double z_6 = 979;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_6);

    // seventh iteration
    n = 7;
    double z_7 = 1008;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_7);

    // eighth iteration
    n = 8;
    double z_8 = 1042;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_8);

    // ninth iteration
    n = 9;
    double z_9 = 1012;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_9);

    // tenth iteration
    n = 10;
    double z_10 = 1011;
    alpha = calculate_gain(n);
    x = calculate_current_estimate(x, alpha, z_10);
}

TEST(alpha_beta_gamma, GoldExample2){
    const std::vector<double> z{1030,989,1017,1009,1013,979,1008,1042,1012,1011};
    std::vector<double> alpha(10,0);
    std::vector<double> x(11,0);
    x[0] = z[0]; // initial
    for(int n = 0; n < 10; ++n){
        alpha[n] = calculate_gain(n+1);
        x[n+1] = calculate_current_estimate(x[n], alpha[n], z[n]);
    }
}

TEST(alpha_beta_gamma, PlaneConstVelExample){
    // delta_t: 5 seconds
    const double delta_t = 5;

    // very imprecise radars
    const double alpha = 0.2;
    const double beta = 0.1;

    struct SystemState{
        double position;
        double speed;
    };

    const std::function<SystemState(const SystemState&, const double&)> estimate = \
        [&delta_t, &alpha, &beta]
        (const SystemState& systemState, const double& measurement)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.position;
        result.position = systemState.position + alpha*diff;
        result.speed = systemState.speed + beta * diff/delta_t;
        return result;
    };

    const std::function<SystemState(const SystemState&)> predict_next = \
        [&delta_t]
        (const SystemState& state)
    {
        SystemState result;
        result.position = state.position + delta_t * state.speed;
        result.speed = state.speed;
        return result;
    };

    std::vector<SystemState> x(12,{0,0});
    x[0] = {30000, 40};
    x[1] = predict_next(x[0]);
    std::vector<double> z{0,30110,30265,30740,30750,31135,31015,31180,31610,31960,31865};
    for(int n = 1; n<11; ++n){
        const SystemState _x = estimate(x[n], z[n]);
        x[n+1] = predict_next(_x);
    }
}

TEST(alpha_beta_gamma, PlaneAccellExample){
    // delta_t: 5 seconds
    const double delta_t = 5;

    // very imprecise radars
    const double alpha = 0.2;
    const double beta = 0.1;

    struct SystemState{
        double position;
        double speed;
    };

    const std::function<SystemState(const SystemState&, const double&)> estimate = \
        [&delta_t, &alpha, &beta]
        (const SystemState& systemState, const double& measurement)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.position;
        result.position = systemState.position + alpha*diff;
        result.speed = systemState.speed + beta * diff/delta_t;
        return result;
    };

    const std::function<SystemState(const SystemState&)> predict_next = \
        [&delta_t]
        (const SystemState& state)
    {
        SystemState result;
        result.position = state.position + delta_t * state.speed;
        result.speed = state.speed;
        return result;
    };

    std::vector<SystemState> x_predictions(12,{0,0});
    x_predictions[0] = {30000, 50};
    x_predictions[1] = predict_next(x_predictions[0]);
    std::vector<SystemState> x_estimates(12, {0,0});
    const std::vector<double> z{0,30160,30365,30890,31050,31785,32215,33130,34510,36010,37265};
    for(int n = 1; n<11; ++n){
        x_estimates[n] = estimate(x_predictions[n], z[n]);
        x_predictions[n+1] = predict_next(x_estimates[n]);
    }
}

TEST(alpha_beta_gamma, alpha_beta_gamma_filter){
    // delta_t: 5 seconds
    const double delta_t = 5;

    // very imprecise radars
    const double alpha = 0.5;
    const double beta = 0.4;
    const double gamma = 0.1;

    struct SystemState{
        double position;
        double speed;
        double acceleration;
    };

    const std::function<SystemState(const SystemState&, const double&)> estimate = \
        [&delta_t, &alpha, &beta, &gamma]
        (const SystemState& systemState, const double& measurement)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.position;
        result.position = systemState.position + alpha*diff;
        result.speed = systemState.speed + beta * diff/delta_t;
        result.acceleration = systemState.acceleration + gamma * diff / (0.5 * std::pow(delta_t,2));
        return result;
    };

    const std::function<SystemState(const SystemState&)> predict_next = \
        [&delta_t]
        (const SystemState& state)
    {
        SystemState result;
        result.position = state.position + state.speed * delta_t + state.acceleration * std::pow(delta_t,2) * 0.5 ;
        result.speed = state.speed + state.acceleration * delta_t;
        result.acceleration = state.acceleration;
        return result;
    };

    std::vector<SystemState> x_predictions(12,{0,0});
    x_predictions[0] = {30000, 50, 0};
    x_predictions[1] = predict_next(x_predictions[0]);
    std::vector<SystemState> x_estimates(12, {0,0});
    const std::vector<double> z{0,30160,30365,30890,31050,31785,32215,33130,34510,36010,37265};
    for(int n = 1; n<11; ++n){
        x_estimates[n] = estimate(x_predictions[n], z[n]);
        x_predictions[n+1] = predict_next(x_estimates[n]);
    }
}
