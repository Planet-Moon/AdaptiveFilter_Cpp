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

    Vec signal(2e4);
    const double sampels_per_second = 1e4;
    const double frequency = 1; // Hz
    const double amplitude = 1;
    const double phi = 0*1e4; // phase shift in seconds
    for(int i=0; i < signal.size(); ++i){
        const double t = i/sampels_per_second;
        signal[i] = amplitude * std::sin(2*pi*frequency*t+phi);
    }

    Json::Value jsonSignal;
    for(int i=0; i < signal.size(); ++i){
        jsonSignal[i] = signal[i];
    }
    JsonServer jServer(80,jsonSignal);
    jServer.host_blocking();

    return 0;
}
