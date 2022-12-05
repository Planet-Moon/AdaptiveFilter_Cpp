#include "SinusGenerator.h"
#include <cmath>

const float M_PI = acos(-1);

SinusGenerator::SinusGenerator(){}
SinusGenerator::~SinusGenerator(){}

float SinusGenerator::getFrequency() const { return _freq; }
void SinusGenerator::setFrequency(float frequency) {
    _next_freq = frequency;
}

float SinusGenerator::getAmplitude() const { return _amp; }
void SinusGenerator::setAmplitude(float amplitude) { _amp = amplitude; }

float SinusGenerator::getSampleTime() const { return _sample_time; }
void SinusGenerator::setSampleTime(float sample_time) { _sample_time = sample_time; }

float SinusGenerator::getTime() const { return _continuos_time; }

float SinusGenerator::next(){
    float res = _amp * sin(2 * M_PI * _freq * _timestep);
    _timestep += _sample_time;
    if(_timestep > 1/_freq){
        if(_next_freq > 0){
            _freq = _next_freq;
            _timestep = 0;
            _next_freq = -1;
        }
        else{
            int ratio = _timestep/_sample_time;
            _timestep = _sample_time*(_timestep/_sample_time - ratio);
        }
    }
    _continuos_time += _sample_time;
    return res;
}
