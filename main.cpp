#include <iostream>
#include "AdaptiveFIR.h"
#include "Fir.h"
#include "WhiteNoise.h"
#include "JsonServer.h"
#include <cmath>
#include "ExampleFir.h"


const double pi = std::acos(-1);

int main(int argc, char **argv){

    Fir fir(filter_taps);
    // Fir fir({1,2,1});
    AdaptiveFIR AFir(fir.n()+1);
    WhiteNoise noise(0, 0.5);

    long long samples = 1e2;
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

    Vec output(signal.size());
    std::vector<AdaptiveFIR::UpdateStats> adaptiveStats(signal.size());
    for(int i=0; i<output.size(); ++i){
        output[i] = fir.filter(signal_noise[i]);
        adaptiveStats[i] = AFir.update(signal_noise[i], output[i]);
    }

    auto AFir_freqz = AFir.freqz(160);

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

    JsonServer jServer(80,json);
    jServer.host_blocking();

    return 0;
}
