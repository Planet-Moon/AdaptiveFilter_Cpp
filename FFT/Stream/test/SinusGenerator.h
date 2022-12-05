#pragma once

class SinusGenerator{
public:
    SinusGenerator();
    virtual ~SinusGenerator();

    float getFrequency() const;
    void setFrequency(float frequency);

    float getAmplitude() const;
    void setAmplitude(float amplitude);

    float getSampleTime() const;
    void setSampleTime(float sample_time);

    float getTime() const;

    float next();

private:
    float _freq=1;
    float _next_freq=-1;
    float _amp=1;

    float _timestep=0;
    float _sample_time=100;
    float _continuos_time=0;
};
