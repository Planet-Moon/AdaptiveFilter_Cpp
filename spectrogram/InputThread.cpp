#define _USE_MATH_DEFINES
#include "InputThread.h"
#include <cmath>


SinusGenerator::SinusGenerator()
{
    _next_freq = std::numeric_limits<float>::quiet_NaN();
}

SinusGenerator::~SinusGenerator(){

}

float SinusGenerator::getFrequency() const {
    return _freq;
}

void SinusGenerator::setFrequency(float frequency) {
    if(_first){
        _freq = frequency;
    }
    else{
        _next_freq = frequency;
    }
}

float SinusGenerator::getAmplitude() const { return _amp; }
void SinusGenerator::setAmplitude(float amplitude) { _amp = amplitude; }

float SinusGenerator::next(){
    if(_first){
        _start_time = std::chrono::steady_clock::now();
        _first = false;
    }
    const auto _now = std::chrono::steady_clock::now();
    const std::chrono::nanoseconds t_delta = _now - _start_time;
    const double time_step = std::chrono::duration_cast<std::chrono::duration<double>>(t_delta).count();
    const float result = _amp * sin(2 * M_PI * _freq * time_step);

    const bool next_nan = isnan(_next_freq);

    if(!next_nan && (signbit(result) != signbit(_last))){
        _freq = _next_freq;
        _next_freq = std::numeric_limits<float>::quiet_NaN();
        _start_time = std::chrono::steady_clock::now();
        if(signbit(result) && !signbit(_last)){
            _start_time += std::chrono::nanoseconds(static_cast<long long>(500'000'000/_freq));
        }
    }

    _last = result;
    return result;
}

InputThread::InputThread(){
    _sinusGenerator = std::make_unique<SinusGenerator>();
    _sinusGenerator->setAmplitude(1);
    _sinusGenerator->setFrequency(500);

    _noiseGenerator = std::make_unique<WhiteNoise>(0.1f, 0.1f);
}

InputThread::~InputThread(){

}

double InputThread::next(){
    return 1.0 * _sinusGenerator->next() + 1.0 * _noiseGenerator->generate();
}

SinusGenerator* InputThread::getSinusGenerator(){
    return _sinusGenerator.get();
}
WhiteNoise* InputThread::getWhiteNoiseGenerator(){
    return _noiseGenerator.get();
}
