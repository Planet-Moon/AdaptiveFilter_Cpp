#include <algorithm>
#include <iostream>
#include <AdaptiveRLSOrder.h>
#include <FirOrder.h>
#include <WhiteNoise.h>
#include <JsonServer.h>
#include <cmath>
#include <chrono>
#include "ExampleFirOrder.h"
#include "MatrixVector.h"
#include <memory>
#include <omp.h>
#include <string>

using steady_clock = std::chrono::steady_clock;

#define N_RUNS  100
#define samples 1'000
#define filterOrder 24

std::chrono::milliseconds time_now() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(steady_clock::now().time_since_epoch());
}

std::string time_now_str(){
    return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(time_now()).count());
}

std::string time_str(const std::chrono::milliseconds& milliseconds) {
    return std::to_string(milliseconds.count());
}

namespace MV = MatrixVector;

const double pi = std::acos(-1);

int main(int argc, char **argv){
    std::cout<< "Number of threads: " << omp_get_num_threads() << std::endl;
    std::chrono::milliseconds time_start = time_now();
    std::cout<< time_str(time_start) << " - Program start" << std::endl;

    // Fir fir(filter_taps7);
    const MV::Vec<filterOrder> coefficients = {5.0, 4.5, 4.0, 3.0, 2.0, 1.0, 0.5, 1.0, 1.5,
        2.0, 2.5, 3.0, -3.0, -2.5, -2.0, -1.5,
        -1.0, -0.5, -1.0, -2.0, -3.0, -4.0, -4.5, -5.0};
    FirOrder fir(coefficients);

    auto error_mat = std::make_unique<MV::Mat<N_RUNS, samples>>();
    MV::init_matrix(*error_mat, 0.0);

    auto b_mat = std::make_unique<MV::Mat3<N_RUNS, samples, filterOrder>>();
    init_matrix3(*b_mat);

    auto input_mat = std::make_unique<MV::Mat<N_RUNS, samples>>();
    MV::init_matrix(*input_mat, 0.0);

    auto output_mat = std::make_unique<MV::Mat<N_RUNS, samples>>();
    MV::init_matrix(*output_mat, 0.0);

    auto firstFilterStats = std::unique_ptr<std::array<UpdateStatsOrder<filterOrder>, samples>>();


    // Fir fir({1,2,1});

    {
        MV::Vec<samples> signal = MV::create_vector<samples>(0.0);
        const double sampels_per_second = 1e4;
        const double frequency = 10; // Hz
        const double amplitude = 1;
        const double phi = 0*1e4; // phase shift in seconds
        #pragma omp parallel for
        for(int i=0; i < signal.size(); ++i){
            const double t = i/sampels_per_second;
            signal[i] = amplitude * std::sin(2*pi*frequency*t+phi);
        }
    }

    for(int n=0; n < N_RUNS; ++n){
        WhiteNoise noise(0, 0.5);

        MV::Vec<samples> signal_noise;
        for(int i=0; i<signal_noise.size(); ++i){
            signal_noise[i] = noise.generate();
        }

        AdaptiveRLSOrder<filterOrder> AFir(0.95);
        std::cout << time_now_str() << " - Running filters " << n << " in thread " << omp_get_thread_num() << std::endl;
        MV::Vec<samples> output;
        auto adaptiveStats = std::make_unique<std::array<UpdateStatsOrder<filterOrder>, samples>>();

        for(int i=0; i<samples; ++i){
            output[i] = fir.filter(signal_noise[i]);
            const auto stats = AFir.update(signal_noise[i], output[i]);
            (*error_mat)[n][i] = stats.error;
            (*b_mat)[n][i] = stats.b;
            (*input_mat)[n][i] = signal_noise[i];
            (*output_mat)[n][i] = stats.y;
            (*adaptiveStats)[i] = stats;
        }
        if(n==0){
            firstFilterStats = std::move(adaptiveStats);
        }

        std::cout << time_now_str() << " - End filters " << n << std::endl;
    }
    std::chrono::milliseconds time_end = time_now();
    std::chrono::milliseconds time_elapsed = time_end - time_start;
    std::cout << "Calculation took " << time_str(time_elapsed) << " ms" << std::endl;

    const MV::Vec<samples> error = MV::Matrix::mean<MV::Matrix::MeanDim::Rows>(*error_mat)[0];
    MV::Mat<samples, filterOrder> b;
    MV::Matrix::mean<MV::Matrix::MeanDim3::dim3Pages>(*b_mat, b);
    const auto input = MV::Matrix::mean<MV::Matrix::MeanDim::Rows>(*input_mat)[0];
    const auto output = MV::Matrix::mean<MV::Matrix::MeanDim::Rows>(*output_mat)[0];

    std::cout << time_now_str() << " - Calculating frequency response" << std::endl;
    const int freqz_points{5000};
    auto AFir_freqz = FirOrder<filterOrder>::freqz(b[b.size() - 1], freqz_points);
    auto Fir_freqz = FirOrder<filterOrder>::freqz(coefficients, freqz_points);

    std::cout << time_now_str() << " - Creating Json response" << std::endl;


    Json::Value json;
    json["input"] = JsonServer::fromVector(input);
    json["output"] = JsonServer::fromVector(output);

    Json::Value jsonUpdateStats{};
    for(int i=0; i<firstFilterStats->size(); ++i){
        (*firstFilterStats)[i].error = error[i];
        jsonUpdateStats[i] = (*firstFilterStats)[i].toJson();
    }
    json["update_stats"] = jsonUpdateStats;

    Json::Value jsonFilterParams{};
    jsonFilterParams["system"] = JsonServer::fromVector(fir.get_b());
    jsonFilterParams["identification"] = JsonServer::fromVector(b[b.size()-1]);
    json["filter_parameters"] = jsonFilterParams;

    MV::Mat<filterOrder, samples> b_t;
    MV::Matrix::transpose(b, b_t);
    json["filter_parameters_time"] = JsonServer::fromMatrix(b_t);

    {
        Json::Value jsonFreqz{};
        jsonFreqz["w"] = JsonServer::fromVector(AFir_freqz.w);
        jsonFreqz["h"] = JsonServer::fromVector(AFir_freqz.h_toStringVec());
        json["adaptive_freqz"] = jsonFreqz;
    }

    {
        Json::Value jsonFreqz{};
        jsonFreqz["w"] = JsonServer::fromVector(Fir_freqz.w);
        jsonFreqz["h"] = JsonServer::fromVector(Fir_freqz.h_toStringVec());
        json["expected_freqz"] = jsonFreqz;
    }

    std::cout << "Size of json: " << sizeof(json) << " bytes" << std::endl;

    std::cout << time_now_str() << " - Running server" << std::endl;
    JsonServer jServer(80,json);
    jServer.host_blocking(true);

    return 0;
}
