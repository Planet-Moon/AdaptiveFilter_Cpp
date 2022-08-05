#include <iostream>
#include <AdaptiveLMS.h>
#include <WhiteNoise.h>
#include <JsonServer.h>
#include <cmath>
#include <chrono>
#include "ExampleFir.h"
#include "ExampleFir2.h"

using steady_clock = std::chrono::steady_clock;

std::string time_now(){
    return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(steady_clock::now().time_since_epoch()).count());
}

const double pi = std::acos(-1);

int main(int argc, char **argv){

    std::cout<< time_now() << " - Program start" << std::endl;

    const int N_RUNS = 1e2;
    const long long samples = 1e3;

    // Fir fir(filter_taps7);
    Fir fir({1, 0.5, 0});
    const int n_adaptive_filter = fir.get_n()+1;

    Mat error_mat = Matrix::zeros(N_RUNS, samples);
    Mat3 b_mat = Matrix::zeros(N_RUNS, samples, n_adaptive_filter);
    Mat input_mat = Matrix::zeros(N_RUNS, samples);
    Mat output_mat = Matrix::zeros(N_RUNS, samples);

    std::vector<UpdateStats> firstFilterStats;

    // Fir fir({1,2,1});

    {
        Vec signal(samples);
        const double sampels_per_second = 1e4;
        const double frequency = 10; // Hz
        const double amplitude = 1;
        const double phi = 0*1e4; // phase shift in seconds
        for(int i=0; i < signal.size(); ++i){
            const double t = i/sampels_per_second;
            signal[i] = amplitude * std::sin(2*pi*frequency*t+phi);
        }
    }

    for(int n=0; n < N_RUNS; ++n){
        WhiteNoise noise(0, 0.5);

        Vec signal_noise(samples);
        for(int i=0; i<signal_noise.size(); ++i){
            signal_noise[i] = noise.generate();
        }

        AdaptiveLMS AFir(n_adaptive_filter, 0.2);

        std::cout << time_now() << " - Running filters " << n << std::endl;
        Vec output(samples);
        std::vector<UpdateStats> adaptiveStats(samples);
        for(int i=0; i<samples; ++i){
            output[i] = fir.filter(signal_noise[i]);
            const auto stats = AFir.update(signal_noise[i], output[i]);
            error_mat[n][i] = stats.error;
            b_mat[n][i] = stats.b;
            input_mat[n][i] = signal_noise[i];
            output_mat[n][i] = stats.y;
            adaptiveStats[i] = stats;
        }
        if(n==0){
            firstFilterStats = adaptiveStats;
        }
        std::cout << time_now() << " - End filters " << n << std::endl;
    }

    const auto error = Matrix::mean(error_mat, 1)[0];
    const auto b = Matrix::mean(b_mat, 3);
    const auto input = Matrix::mean(input_mat, 1)[0];
    const auto output = Matrix::mean(output_mat, 1)[0];

    std::cout << time_now() << " - Calculating frequency response" << std::endl;
    auto AFir_freqz = AdaptiveLMS::freqz(b[b.size()-1], 160);

    std::cout << time_now() << " - Creating Json response" << std::endl;

    Json::Value json;
    json["input"] = JsonServer::fromVector(input);
    json["output"] = JsonServer::fromVector(output);

    Json::Value jsonUpdateStats{};
    for(int i=0; i<firstFilterStats.size(); ++i){
        firstFilterStats[i].error = error[i];
        jsonUpdateStats[i] = firstFilterStats[i].toJson();
    }
    json["update_stats"] = jsonUpdateStats;

    Json::Value jsonFilterParams{};
    jsonFilterParams["system"] = JsonServer::fromVector(fir.get_b());
    jsonFilterParams["identification"] = JsonServer::fromVector(b[b.size()-1]);
    json["filter_parameters"] = jsonFilterParams;
    json["filter_parameters_time"] = JsonServer::fromMatrix(Matrix::transpose(b));

    Json::Value jsonFreqz{};
    jsonFreqz["w"] = JsonServer::fromVector(AFir_freqz.w);
    jsonFreqz["h"] = JsonServer::fromVector(AFir_freqz.h_toStringVec());
    json["freqz"] = jsonFreqz;

    std::cout << "Size of json: " << sizeof(json) << " bytes" << std::endl;

    std::cout << time_now() << " - Running server" << std::endl;
    JsonServer jServer(80,json);
    jServer.host_blocking(true);

    return 0;
}
