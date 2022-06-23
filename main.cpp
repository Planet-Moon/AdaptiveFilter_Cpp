#include <iostream>
#include "AdaptiveFIR.h"
#include "Fir.h"
#include "WhiteNoise.h"
#include "JsonServer.h"
#include <cmath>


const double pi = std::acos(-1);

int main(int argc, char **argv){

    AdaptiveFIR AFir(6);
    Fir fir({1,1,1,1,1});
    WhiteNoise noise(0, 0.3);

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

    Vec output(signal.size());
    std::vector<AdaptiveFIR::UpdateStats> adaptiveStats(signal.size());
    for(int i=0; i<output.size(); ++i){
        output[i] = fir.filter(signal_noise[i]);
        adaptiveStats[i] = AFir.update(output[i], signal_noise[i]);
    }

    Json::Value json;
    json["input"] = JsonServer::fromVector(signal_noise);
    json["output"] = JsonServer::fromVector(output);
    Json::Value jsonUpdateStats{};
    for(int i=0; i<adaptiveStats.size(); ++i){
        jsonUpdateStats[i] = adaptiveStats[i].toJson();
    }
    json["update_stats"] = jsonUpdateStats;
    JsonServer jServer(80,json);
    jServer.host_blocking();

    return 0;
}
