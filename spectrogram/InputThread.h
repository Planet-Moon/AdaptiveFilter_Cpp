#pragma once
#include <chrono>
#include <memory>
#include <limits>
#include <thread>
#include <WhiteNoise.h>


class SinusGenerator{
public:
    SinusGenerator();
    virtual ~SinusGenerator();

    float getFrequency() const;
    void setFrequency(float frequency);

    float getAmplitude() const;
    void setAmplitude(float amplitude);

    virtual float next();

private:
    std::chrono::steady_clock::time_point _start_time;
    bool _first{true};

    float _freq = 1;
    float _next_freq = 0;
    float _amp = 1;

    float _last = 0;
};

class InputThread{
public:
    InputThread();
    ~InputThread();
    double next();

    SinusGenerator* getSinusGenerator();
    WhiteNoise* getWhiteNoiseGenerator();

private:
    std::unique_ptr<SinusGenerator> _sinusGenerator;
    std::unique_ptr<WhiteNoise> _noiseGenerator;

    std::thread _sample_thread;
    bool _sample_thread_run{true};
};
