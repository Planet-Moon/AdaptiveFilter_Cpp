#include "gtest/gtest.h"
#include <kalman.h>
#include <functional>

TEST(Kalman_1D, Template){
    // delta_t: 5 seconds
    const double delta_t = 5;

    // measurement uncertainty
    const double measurement_uncertainty = 0.1;

    struct SystemState{
        double position;
        double speed;
    };

    const std::function<SystemState(const SystemState&, const double&, const double&, const int&)> state_update = \
        [&delta_t]
        (const SystemState& systemState, const double& measurement, const double& kalman_gain, const int& n)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.position;
        result.position = systemState.position + kalman_gain*diff;
        result.speed = systemState.speed + kalman_gain * diff/delta_t;
        return result;
    };

    const std::function<SystemState(const SystemState&)> state_extrapolation = \
        [&delta_t]
        (const SystemState& state)
    {
        SystemState result;
        result.position = state.position + delta_t * state.speed;
        result.speed = state.speed;
        return result;
    };

    const std::function<double(const double&, const double&)> kalman_gain = \
        [](const double& estimate_uncertainty, const double& measurement_uncertainty)->double
    {
        return estimate_uncertainty/(estimate_uncertainty + measurement_uncertainty);
    };

    const std::function<double(const double&, const double&)> covariance_update = \
        [](const double& kalman_gain, const double& estimate_uncertainty)->double
    {
        return (1 - kalman_gain)*estimate_uncertainty;
    };

    const std::function<double(const double&)> covariance_extrapolation = \
        [](const double& covariance)
    {
        return covariance;
    };
}

TEST(Kalman, Building){
    // height of building in meters
    const std::vector<double> z = {0, 48.54, 47.11, 55.01, 55.15, 49.89, 40.85, 46.72, 50.05, 51.27, 49.95};

    struct SystemState{
        double height;
    };

    const std::function<SystemState(const SystemState&, const double&, const double&)> state_update = \
        [](const SystemState& systemState, const double& measurement, const double& kalman_gain)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.height;
        result.height = systemState.height + kalman_gain*diff;
        return result;
    };

    const std::function<SystemState(const SystemState&)> state_extrapolation = \
        [](const SystemState& state)
    {
        SystemState result = state;
        return result;
    };

    const std::function<double(const double&, const double&)> kalman_gain = \
        [](const double& estimate_uncertainty, const double& measurement_uncertainty)->double
    {
        return estimate_uncertainty/(estimate_uncertainty + measurement_uncertainty);
    };

    const std::function<double(const double&, const double&)> covariance_update = \
        [](const double& kalman_gain, const double& estimate_uncertainty)->double
    {
        return (1 - kalman_gain)*estimate_uncertainty;
    };

    const std::function<double(const double&)> covariance_extrapolation = \
        [](const double& covariance)
    {
        return covariance;
    };

    std::vector<SystemState> x_predictions(12, {0});
    x_predictions[0] = {60};
    x_predictions[1] = state_extrapolation(x_predictions[0]);
    std::vector<SystemState> x_estimates(11, {0});

    std::vector<double> estimate_uncertainty(12,0);
    estimate_uncertainty[0] = 225; // +-15m unsicherheit quadriert
    estimate_uncertainty[1] = covariance_extrapolation(estimate_uncertainty[0]);
    const double altimeter_uncertainty = 25; // standard deviation 5

    std::vector<double> k(11, 0);

    for(int n = 1; n<11; ++n){
        k[n] = kalman_gain(estimate_uncertainty[n], altimeter_uncertainty);
        x_estimates[n] = state_update(x_predictions[n], z[n], k[n]);
        estimate_uncertainty[n+1] = covariance_update(k[n], estimate_uncertainty[n]);
        x_predictions[n+1] = state_extrapolation(x_estimates[n]);
        continue;
    }
}

TEST(Kalman, LiquidTemperature){
    // measurements are taken every 5 seconds
    const double delta_t = 5;

    // real temperature of the liquid
    const std::vector<double> _z = {0,49.979,50.025,50,50.003,19.994,50.002,49.999,50.006,49.998,49.991};
    // measured temperature of the liquid
    const std::vector<double> z =  {0,49.95,49.967,50.1,50.106,49.992,49.819,49.933,50.007,50.023,49.99};


    struct SystemState{
        double temperature;
    };

    const std::function<SystemState(const SystemState&, const double&, const double&)> state_update = \
        [](const SystemState& systemState, const double& measurement, const double& kalman_gain)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.temperature;
        result.temperature = systemState.temperature + kalman_gain*diff;
        return result;
    };

    const std::function<SystemState(const SystemState&)> state_extrapolation = \
        [](const SystemState& state)
    {
        SystemState result = state;
        return result;
    };

    const std::function<double(const double&, const double&)> kalman_gain = \
        [](const double& estimate_uncertainty, const double& measurement_uncertainty)->double
    {
        return estimate_uncertainty/(estimate_uncertainty + measurement_uncertainty);
    };

    const std::function<double(const double&, const double&)> covariance_update = \
        [](const double& kalman_gain, const double& estimate_uncertainty)->double
    {
        return (1 - kalman_gain)*estimate_uncertainty;
    };

    const std::function<double(const double&, const double&)> covariance_extrapolation = \
        [](const double& covariance, const double& process_noise)
    {
        return covariance + process_noise;
    };

    std::vector<SystemState> x_predictions(12, {0});
    x_predictions[0] = {10};
    x_predictions[1] = state_extrapolation(x_predictions[0]);
    std::vector<SystemState> x_estimates(11, {0});

    const double measurement_uncertainty = std::pow(0.1, 2);// standard deviation of measurement error (0.1)
    const double process_noise = 0.0001;
    std::vector<double> estimate_uncertainty(12,0);
    estimate_uncertainty[0] = 10'000; // +-10°C unsicherheit quadriert
    estimate_uncertainty[1] = covariance_extrapolation(estimate_uncertainty[0], process_noise);


    std::vector<double> k(11, 0);

    for(int n = 1; n<11; ++n){
        k[n] = kalman_gain(estimate_uncertainty[n], measurement_uncertainty);
        x_estimates[n] = state_update(x_predictions[n], z[n], k[n]);
        const auto p_nn = covariance_update(k[n], estimate_uncertainty[n]);
        estimate_uncertainty[n+1] = covariance_extrapolation(p_nn, process_noise);
        x_predictions[n+1] = state_extrapolation(x_estimates[n]);
        continue;
    }
}

TEST(Kalman, HeatingLiquidTemperature1){
    // measurements are taken every 5 seconds
    const double delta_t = 5;

    // real temperature of the liquid
    const std::vector<double> _z = {0, 50.479, 51.025, 51.5, 52.003, 52.494, 53.002, 53.499, 54.006, 54.498, 54.991};
    // measured temperature of the liquid
    const std::vector<double> z =  {0, 50.45, 50.967, 51.6, 52.106, 52.492, 52.819, 53.433, 54.007, 54.523, 54.99};


    struct SystemState{
        double temperature;
    };

    const std::function<SystemState(const SystemState&, const double&, const double&)> state_update = \
        [](const SystemState& systemState, const double& measurement, const double& kalman_gain)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.temperature;
        result.temperature = systemState.temperature + kalman_gain*diff;
        return result;
    };

    const std::function<SystemState(const SystemState&)> state_extrapolation = \
        [](const SystemState& state)
    {
        SystemState result = state;
        return result;
    };

    const std::function<double(const double&, const double&)> kalman_gain = \
        [](const double& estimate_uncertainty, const double& measurement_uncertainty)->double
    {
        return estimate_uncertainty/(estimate_uncertainty + measurement_uncertainty);
    };

    const std::function<double(const double&, const double&)> covariance_update = \
        [](const double& kalman_gain, const double& estimate_uncertainty)->double
    {
        return (1 - kalman_gain)*estimate_uncertainty;
    };

    const std::function<double(const double&, const double&)> covariance_extrapolation = \
        [](const double& covariance, const double& process_noise)
    {
        return covariance + process_noise;
    };

    std::vector<SystemState> x_predictions(12, {0});
    x_predictions[0] = {10};
    x_predictions[1] = state_extrapolation(x_predictions[0]);
    std::vector<SystemState> x_estimates(11, {0});

    const double measurement_uncertainty = std::pow(0.1, 2); // standard deviation of measurement error (0.1)
    const double process_noise = 0.0001;
    std::vector<double> estimate_uncertainty(12,0);
    estimate_uncertainty[0] = 10'000; // +-10°C unsicherheit quadriert
    estimate_uncertainty[1] = covariance_extrapolation(estimate_uncertainty[0], process_noise);

    std::vector<double> k(11, 0);

    for(int n = 1; n<11; ++n){
        k[n] = kalman_gain(estimate_uncertainty[n], measurement_uncertainty);
        x_estimates[n] = state_update(x_predictions[n], z[n], k[n]);
        const auto p_nn = covariance_update(k[n], estimate_uncertainty[n]);
        estimate_uncertainty[n+1] = covariance_extrapolation(p_nn, process_noise);
        x_predictions[n+1] = state_extrapolation(x_estimates[n]);
        continue;
    }
}

TEST(Kalman, HeatingLiquidTemperature2){
    // measurements are taken every 5 seconds
    const double delta_t = 5;

    // real temperature of the liquid
    const std::vector<double> _z = {0, 50.479, 51.025, 51.5, 52.003, 52.494, 53.002, 53.499, 54.006, 54.498, 54.991};
    // measured temperature of the liquid
    const std::vector<double> z =  {0, 50.45, 50.967, 51.6, 52.106, 52.492, 52.819, 53.433, 54.007, 54.523, 54.99};


    struct SystemState{
        double temperature;
    };

    const std::function<SystemState(const SystemState&, const double&, const double&)> state_update = \
        [](const SystemState& systemState, const double& measurement, const double& kalman_gain)->SystemState
    {
        SystemState result;
        const double diff = measurement - systemState.temperature;
        result.temperature = systemState.temperature + kalman_gain*diff;
        return result;
    };

    const std::function<SystemState(const SystemState&)> state_extrapolation = \
        [](const SystemState& state)
    {
        SystemState result = state;
        return result;
    };

    const std::function<double(const double&, const double&)> kalman_gain = \
        [](const double& estimate_uncertainty, const double& measurement_uncertainty)->double
    {
        return estimate_uncertainty/(estimate_uncertainty + measurement_uncertainty);
    };

    const std::function<double(const double&, const double&)> covariance_update = \
        [](const double& kalman_gain, const double& estimate_uncertainty)->double
    {
        return (1 - kalman_gain)*estimate_uncertainty;
    };

    const std::function<double(const double&, const double&)> covariance_extrapolation = \
        [](const double& covariance, const double& process_noise)
    {
        return covariance + process_noise;
    };

    std::vector<SystemState> x_predictions(12, {0});
    x_predictions[0] = {10};
    x_predictions[1] = state_extrapolation(x_predictions[0]);
    std::vector<SystemState> x_estimates(11, {0});

    const double measurement_uncertainty = std::pow(0.1, 2); // standard deviation of measurement error (0.1)
    const double process_noise = 0.15; // q
    std::vector<double> estimate_uncertainty(12,0);
    estimate_uncertainty[0] = 10'000; // +-10°C unsicherheit quadriert
    estimate_uncertainty[1] = covariance_extrapolation(estimate_uncertainty[0], process_noise);

    std::vector<double> k(11, 0);

    for(int n = 1; n<11; ++n){
        k[n] = kalman_gain(estimate_uncertainty[n], measurement_uncertainty);
        x_estimates[n] = state_update(x_predictions[n], z[n], k[n]);
        const auto p_nn = covariance_update(k[n], estimate_uncertainty[n]);
        estimate_uncertainty[n+1] = covariance_extrapolation(p_nn, process_noise);
        x_predictions[n+1] = state_extrapolation(x_estimates[n]);
        continue;
    }
}
