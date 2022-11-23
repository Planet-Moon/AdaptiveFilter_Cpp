#include <string>
#include <iostream>
#include <json/json.h>
#include <thread>

const float M_PI = acos(-1);

class SinusGenerator{
public:
    SinusGenerator(){}

    float getFrequency() const { return _freq; }
    void setFrequency(float frequency) { _freq = frequency; }

    float getAmplitude() const { return _amp; }
    void setAmplitude(float amplitude) { _amp = amplitude; }

    float getSampleTime() const { return _sample_time; }
    void setSampleTime(float sample_time) { _sample_time = sample_time; }

    float next(){
        float res = _amp * sin(2 * M_PI * _freq * _timestep);
        _timestep += _sample_time;
        if(_timestep > 1/_freq){
            int ratio = _timestep/_sample_time;
            _timestep = _sample_time*(_timestep/_sample_time - ratio);
        }
        return res;
    }

private:
    float _freq=1;
    float _amp=1;

    float _timestep=0;
    float _sample_time=100;
};

int main(int argc, char **argv){
    SinusGenerator sinusGenerator;
    sinusGenerator.setAmplitude(1);
    sinusGenerator.setFrequency(1);
    sinusGenerator.setSampleTime(0.01);
    int counter = 0;
    while(counter > -1){
        std::cout << sinusGenerator.next() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        counter++;
    }
    return 0;
}
