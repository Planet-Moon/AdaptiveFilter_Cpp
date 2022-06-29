#include <iostream>
#include "AdaptiveFIR.h"
#include "Fir.h"
#include "WhiteNoise.h"
#include "JsonServer.h"
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

    Fir fir(filter_taps7);
    // Fir fir({1,2,1});
    AdaptiveFIR AFir(fir.n()+1);
    WhiteNoise noise(0, 0.5);

    long long samples = 1e3;
    Vec signal(samples);
    const double sampels_per_second = 1e4;
    const double frequency = 10; // Hz
    const double amplitude = 1;
    const double phi = 0*1e4; // phase shift in seconds
    for(int i=0; i < signal.size(); ++i){
        const double t = i/sampels_per_second;
        signal[i] = amplitude * std::sin(2*pi*frequency*t+phi);
    }

    Vec signal_noise = signal;
    for(int i=0; i<signal_noise.size(); ++i){
        signal_noise[i] = noise.generate();
    }

    std::cout << time_now() << " - Running filters" << std::endl;
    Vec output(signal.size());
    std::vector<AdaptiveFIR::UpdateStats> adaptiveStats(signal.size());
    for(int i=0; i<output.size(); ++i){
        output[i] = fir.filter(signal_noise[i]);
        adaptiveStats[i] = AFir.update(signal_noise[i], output[i]);
    }

    std::cout << time_now() << " - Calculating frequency response" << std::endl;
    auto AFir_freqz = AFir.freqz(160);

    std::cout << time_now() << " - Creating Json response" << std::endl;

    Json::Value json;
    json["input"] = JsonServer::fromVector(signal_noise);
    json["output"] = JsonServer::fromVector(output);

    Json::Value jsonUpdateStats{};
    for(int i=0; i<adaptiveStats.size(); ++i){
        jsonUpdateStats[i] = adaptiveStats[i].toJson();
    }
    json["update_stats"] = jsonUpdateStats;

    Json::Value jsonFilterParams{};
    jsonFilterParams["system"] = JsonServer::fromVector(fir.b());
    jsonFilterParams["identification"] = JsonServer::fromVector(AFir.get_b());
    json["filter_parameters"] = jsonFilterParams;

    Json::Value jsonFreqz{};
    jsonFreqz["w"] = JsonServer::fromVector(AFir_freqz.w);
    jsonFreqz["h"] = JsonServer::fromVector(AFir_freqz.h_toStringVec());
    json["freqz"] = jsonFreqz;

    std::cout << "Size of json: " << sizeof(json) << " bytes" << std::endl;

    std::cout << time_now() << " - Running server" << std::endl;
    JsonServer jServer(80,json);
    jServer.host_blocking();

    return 0;
}
