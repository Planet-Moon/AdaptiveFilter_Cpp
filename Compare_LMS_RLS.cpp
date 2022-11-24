#include <iostream>
#include <AdaptiveRLS.h>
#include <AdaptiveLMS.h>
#include <Fir.h>
#include <WhiteNoise.h>
#include <JsonServer.h>
#include <cmath>
#include <chrono>
#include "ExampleFir.h"
#include "ExampleFir2.h"
#include <omp.h>

using steady_clock = std::chrono::steady_clock;

std::string time_now(){
    return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(steady_clock::now().time_since_epoch()).count());
}

const double pi = std::acos(-1);

struct FilterInfo{
    FilterInfo(int N_RUNS, long long samples, int n_adaptive_filter){
        error_mat = Matrix::zeros(N_RUNS, samples);
        b_mat = Matrix::zeros(N_RUNS, samples, n_adaptive_filter);
        input_mat = Matrix::zeros(N_RUNS, samples);
        output_mat = Matrix::zeros(N_RUNS, samples);
    }
    Mat error_mat;
    Mat3 b_mat;
    Mat input_mat;
    Mat output_mat;
};

int main(int argc, char **argv){

    std::cout<< "Number of threads: " << omp_get_num_threads() << std::endl;
    std::cout<< time_now() << " - Program start" << std::endl;

    const int N_RUNS = 1e2;
    const long long samples = 1e4;

    Fir fir(filter_taps7);
    const Vec coefficients = {0.2, 0.2, 0.2, 0.2, 0.2};
    // Fir fir(coefficients);
    const int n_adaptive_filter = fir.get_n();

    FilterInfo ALMS_info(N_RUNS, samples, n_adaptive_filter);
    FilterInfo ARLS_info(N_RUNS, samples, n_adaptive_filter);

    std::vector<UpdateStats> ALMS_firstFilterStats;
    std::vector<UpdateStats> ARLS_firstFilterStats;

    // Fir fir({1,2,1});

    {
        Vec signal(samples);
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

        Vec signal_noise(samples);
        for(int i=0; i<signal_noise.size(); ++i){
            signal_noise[i] = noise.generate();
        }

        AdaptiveLMS ALMS_Fir(n_adaptive_filter, 0.02);
        AdaptiveRLS ARLS_Fir(n_adaptive_filter);

        std::cout << time_now() << " - Running filters " << n << " in thread " << omp_get_thread_num() << std::endl;
        Vec output(samples);
        std::vector<UpdateStats> ALMS_adaptiveStats(samples);
        std::vector<UpdateStats> ARLS_adaptiveStats(samples);
        for(int i=0; i<samples; ++i){
            output[i] = fir.filter(signal_noise[i]);
            { // LMS update
                const auto stats = ALMS_Fir.update(signal_noise[i], output[i]);
                ALMS_info.error_mat[n][i] = stats.error;
                ALMS_info.b_mat[n][i] = stats.b;
                ALMS_info.input_mat[n][i] = signal_noise[i];
                ALMS_info.output_mat[n][i] = stats.y;
                ALMS_adaptiveStats[i] = stats;
            }
            { // RLS update
                const auto stats = ARLS_Fir.update(signal_noise[i], output[i]);
                ARLS_info.error_mat[n][i] = stats.error;
                ARLS_info.b_mat[n][i] = stats.b;
                ARLS_info.input_mat[n][i] = signal_noise[i];
                ARLS_info.output_mat[n][i] = stats.y;
                ARLS_adaptiveStats[i] = stats;
            }
        }
        if(n==0){
            ALMS_firstFilterStats = ALMS_adaptiveStats;
            ARLS_firstFilterStats = ARLS_adaptiveStats;
        }
        std::cout << time_now() << " - End filters " << n << std::endl;
    }

    Json::Value json;
    { // Calc stuff for LMS
        const auto error = Matrix::mean(ALMS_info.error_mat, 1)[0];
        const auto b = Matrix::mean(ALMS_info.b_mat, 3);
        const auto input = Matrix::mean(ALMS_info.input_mat, 1)[0];
        const auto output = Matrix::mean(ALMS_info.output_mat, 1)[0];

        std::cout << time_now() << " - Calculating frequency response" << std::endl;
        const int freqz_points{5000};
        auto AFir_freqz = Fir::freqz(b[b.size() - 1], freqz_points);
        auto Fir_freqz = Fir::freqz(coefficients, freqz_points);

        std::cout << time_now() << " - Creating Json response" << std::endl;

        Json::Value jsonLMS;
        jsonLMS["input"] = JsonServer::fromVector(input);
        jsonLMS["output"] = JsonServer::fromVector(output);

        Json::Value jsonUpdateStats{};
        for(int i=0; i<ALMS_firstFilterStats.size(); ++i){
            ALMS_firstFilterStats[i].error = error[i];
            jsonUpdateStats[i] = ALMS_firstFilterStats[i].toJson();
        }
        jsonLMS["update_stats"] = jsonUpdateStats;

        Json::Value jsonFilterParams{};
        jsonFilterParams["system"] = JsonServer::fromVector(fir.get_b());
        jsonFilterParams["identification"] = JsonServer::fromVector(b[b.size()-1]);
        jsonLMS["filter_parameters"] = jsonFilterParams;
        jsonLMS["filter_parameters_time"] = JsonServer::fromMatrix(Matrix::transpose(b));

        {
            Json::Value jsonFreqz{};
            jsonFreqz["w"] = JsonServer::fromVector(AFir_freqz.w);
            jsonFreqz["h"] = JsonServer::fromVector(AFir_freqz.h_toStringVec());
            jsonLMS["adaptive_freqz"] = jsonFreqz;
        }

        {
            Json::Value jsonFreqz{};
            jsonFreqz["w"] = JsonServer::fromVector(Fir_freqz.w);
            jsonFreqz["h"] = JsonServer::fromVector(Fir_freqz.h_toStringVec());
            jsonLMS["expected_freqz"] = jsonFreqz;
        }
        json["LMS"] = jsonLMS;
    }

    { // Calc stuff for RLS
        const auto error = Matrix::mean(ARLS_info.error_mat, 1)[0];
        const auto b = Matrix::mean(ARLS_info.b_mat, 3);
        const auto input = Matrix::mean(ARLS_info.input_mat, 1)[0];
        const auto output = Matrix::mean(ARLS_info.output_mat, 1)[0];

        std::cout << time_now() << " - Calculating frequency response" << std::endl;
        const int freqz_points{5000};
        auto AFir_freqz = Fir::freqz(b[b.size() - 1], freqz_points);
        auto Fir_freqz = Fir::freqz(coefficients, freqz_points);

        std::cout << time_now() << " - Creating Json response" << std::endl;

        Json::Value jsonRLS;
        jsonRLS["input"] = JsonServer::fromVector(input);
        jsonRLS["output"] = JsonServer::fromVector(output);

        Json::Value jsonUpdateStats{};
        for(int i=0; i<ARLS_firstFilterStats.size(); ++i){
            ARLS_firstFilterStats[i].error = error[i];
            jsonUpdateStats[i] = ARLS_firstFilterStats[i].toJson();
        }
        jsonRLS["update_stats"] = jsonUpdateStats;

        Json::Value jsonFilterParams{};
        jsonFilterParams["system"] = JsonServer::fromVector(fir.get_b());
        jsonFilterParams["identification"] = JsonServer::fromVector(b[b.size()-1]);
        jsonRLS["filter_parameters"] = jsonFilterParams;
        jsonRLS["filter_parameters_time"] = JsonServer::fromMatrix(Matrix::transpose(b));

        {
            Json::Value jsonFreqz{};
            jsonFreqz["w"] = JsonServer::fromVector(AFir_freqz.w);
            jsonFreqz["h"] = JsonServer::fromVector(AFir_freqz.h_toStringVec());
            jsonRLS["adaptive_freqz"] = jsonFreqz;
        }

        {
            Json::Value jsonFreqz{};
            jsonFreqz["w"] = JsonServer::fromVector(Fir_freqz.w);
            jsonFreqz["h"] = JsonServer::fromVector(Fir_freqz.h_toStringVec());
            jsonRLS["expected_freqz"] = jsonFreqz;
        }
        json["RLS"] = jsonRLS;
    }


    std::cout << "Size of json: " << sizeof(json) << " bytes" << std::endl;

    std::cout << time_now() << " - Running server" << std::endl;
    JsonServer jServer(80,json);
    jServer.host_blocking(false);

    return 0;
}
