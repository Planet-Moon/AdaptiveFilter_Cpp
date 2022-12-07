#include <thread>
#include <queue>
#include "SinusGenerator.h"
#include "RectGenerator.h"
#include "WhiteNoise.h"
#include "FFT.h"
#include "DataPoint.h"
#include <httplib.h>
#include <iostream>

static void server_thread_func(httplib::Server* server, const char* address, int port){
    server->listen(address, port);
}

static std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
}

template <typename T>
static Json::Value fromVector(const std::vector<T>& vector){
    Json::Value result;
    for(int i = 0; i < vector.size(); i++){
        result[i] = vector[i];
    }
    return result;
}

template <typename T>
static Json::Value fromVector(const std::vector<T>& vector, size_t length){
    Json::Value result;
    for(int i = 0; i < std::min(vector.size(),length); i++){
        result[i] = vector[i];
    }
    return result;
}

template <typename T>
static Json::Value fromVector(const std::vector<std::complex<T>>& vector){
    Json::Value result;
    for(int i = 0; i < vector.size(); i++){
        std::string s = std::to_string(vector[i].real());
        const T& imag = vector[i].imag();
        if(imag < 0){
            s += std::to_string(imag)+"i";
        }
        else{
            s += "+"+std::to_string(imag)+"i";
        }
        result[i] = s;
    }
    return result;
}

template <typename T>
static Json::Value fromVector(const std::vector<std::complex<T>>& vector, size_t length){
    Json::Value result;
    for(int i = 0; i < std::min(vector.size(),length); i++){
        std::string s = std::to_string(vector[i].real());
        const T& imag = vector[i].imag();
        if(imag < 0){
            s += std::to_string(imag)+"i";
        }
        else{
            s += "+"+std::to_string(imag)+"i";
        }
        result[i] = s;
    }
    return result;
}

template<typename Tr, typename T>
std::vector<Tr> linspace(T start_in, T end_in, int num_in)
{
    // https://stackoverflow.com/a/27030598
    std::vector<Tr> linspaced(num_in);

    Tr start = static_cast<Tr>(start_in);
    Tr end = static_cast<Tr>(end_in);
    Tr num = static_cast<Tr>(num_in);


    if (num == 0) { return linspaced; }
    if (num == 1)
    {
        linspaced[0] = start;
        return linspaced;
    }

    Tr delta = (end - start) / (num - 1);

    for(int i=0; i < num-1; ++i){
        linspaced[i] = (start + delta * i);
    }
    linspaced.back() = (end); // I want to ensure that start and end
                              // are exactly the same as the input
    return linspaced;
}


int find_dominant_freq(const std::vector<std::complex<double>>& fft){
    // calculate magnitudes of fft points
    std::vector<double> fft_mag(fft.size());
    std::transform(fft.begin(), fft.end(), fft_mag.begin(),
        [](const std::complex<double>& val){
            return  std::abs(val);
    });

    // find largest magnitude
    int idx = 0;
    float mag_max = 0;
    for(int i = 0; i < fft_mag.size()*0.5; ++i){
        if(fft_mag[i] > mag_max){
            mag_max = fft_mag[i];
            idx = i;
        }
    }

    return idx;
}

static void fft_thread_func(std::queue<std::vector<DataPoint>>* input, size_t* datapoints_got, std::vector<std::complex<double>>* output, bool* ready) {
    while(true) {
        if(!input->empty()) {
            auto input_storage = input->front();
            *output = std::vector<std::complex<double>>(input_storage.size());
            const auto time_start = std::chrono::high_resolution_clock::now();
            auto fft_complex = FFT::fft(DataPoint::toVec<double>(input_storage));
            const auto time_end = std::chrono::high_resolution_clock::now();
            auto diff = time_end - time_start;
            // std::cout << "FFT time: " << diff.count()* 1e-9 << " seconds" << std::endl;
            *output = fft_complex;
            input->pop();
            *ready = true;
        }
        else{
            *ready = false;
        }
    }
}

struct FrequencyChanger{
public:
    FrequencyChanger(SinusGenerator* generator, const float& min_freq, const float& max_freq, const float& change):
        _generator(generator), _min_freq(min_freq), _max_freq(max_freq), _freq_speed(change){
            _next_freq = _generator->getFrequency() + _freq_speed;
    }

    float calculate_next_freq(){
        const auto freq = _generator->getFrequency();

        // reverse direction when boundaries are hit
        if(freq >= _max_freq){
            _direction = false;
        }
        else if(freq <= _min_freq){
            _direction = true;
        }

        // check if _next_freq is the current frequency of the generator
        // change frequency if it has been set by generator
        if(_next_freq == freq){
            if(_direction){
                _next_freq = freq + _freq_speed;
            }
            else{
                _next_freq = freq - _freq_speed;
            }
        }

        // check boundaries of next frequency
        if(_next_freq < _min_freq){
            _next_freq = _min_freq;
        }
        else if(_next_freq > _max_freq){
            _next_freq = _max_freq;
        }

        return _next_freq;
    }

private:
    SinusGenerator* _generator;
    float _min_freq;
    float _max_freq;
    float _freq_speed;
    float _next_freq;
    bool _direction = true;
};

int main(int argc, const char** argv) {
    WhiteNoise noiseGenerator(0, 0.001);

    const double sample_time_seconds = 0.001;
    const double sample_frequency_Hz = 1000;

    SinusGenerator generator;
    generator.setAmplitude(1);
    generator.setFrequency(100);
    generator.setSampleTime(sample_time_seconds);
    const float min_freq = 1;
    const float max_freq = 0.5/sample_time_seconds;
    const float freq_speed = (max_freq-min_freq)/100;
    FrequencyChanger generatorChanger(&generator, min_freq, max_freq, freq_speed);

    RectGenerator rectGenerator;
    rectGenerator.setAmplitude(0);
    rectGenerator.setFrequency(10);
    rectGenerator.setSampleTime(sample_time_seconds);

    const int dataPointsSize = 1500;
    std::queue<std::vector<DataPoint>> dataPoints;
    size_t dataPoints_sampled = 0;

    double time = 0;

    std::vector<std::complex<double>> fft_complex(dataPointsSize);
    const std::vector<double> fft_freq = linspace<double>(0.0, sample_frequency_Hz, dataPointsSize);

    httplib::Server svr;
    float sinus_frequency = 0;
    svr.Get("/data", [&fft_complex, &fft_freq, &sinus_frequency](const httplib::Request &, httplib::Response &res){
        Json::Value json;
        json["fft"] = fromVector(fft_complex, fft_complex.size()/2 -1);
        json["freq"] = fromVector(fft_freq, fft_complex.size()/2 -1);
        json["sinFreq"] = sinus_frequency;
        json["dominantFreq"] = fft_freq[find_dominant_freq(fft_complex)];
        std::string content = convert_to_string(json);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(content, "application/json");
    });
    auto server_thread = std::make_unique<std::thread>(server_thread_func, &svr, "127.0.0.1", 8801);

    bool fft_ready = false;
    auto fft_thread = std::make_unique<std::thread>(fft_thread_func, &dataPoints, &dataPoints_sampled, &fft_complex, &fft_ready);

    std::vector<DataPoint> data(dataPointsSize);
    while (true)
    {
        float next_freq = generatorChanger.calculate_next_freq();

        float signal = generator.next();// + rectGenerator.next() + noiseGenerator.generate();
        data[dataPoints_sampled] = DataPoint(time, signal);
        dataPoints_sampled++;

        if(dataPoints_sampled == dataPointsSize){
            sinus_frequency = generator.getFrequency();
            dataPoints.push(data);
            data = std::vector<DataPoint>(dataPointsSize);
            dataPoints_sampled = 0;
            generator.setFrequency(next_freq);
            rectGenerator.setFrequency(next_freq);
        }

        time += sample_time_seconds;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(sample_time_seconds*1000)));
    }
    return 0;
}
