#include <thread>
#include <queue>
#include "SinusGenerator.h"
#include "RectGenerator.h"
#include "WhiteNoise.h"
#include "FFT.h"
#include "DataPoint.h"
#include <httplib.h>

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


static void fft_thread_func(std::queue<std::vector<DataPoint>>* input, size_t* datapoints_got, std::vector<std::complex<double>>* output, bool* ready) {
    while(true) {
        if(!input->empty()) {
            auto input_storage = input->front();
            *output = std::vector<std::complex<double>>(input_storage.size());
            auto fft_complex = FFT::fft(DataPoint::toVec<double>(input_storage));
            *output = fft_complex;
            input->pop();
            *ready = true;
        }
        else{
            *ready = false;
        }
    }
}

int main(int argc, const char** argv) {
    WhiteNoise noiseGenerator(0, 0.5);

    const double sample_time_seconds = 0.001;
    const double sample_frequency_Hz = 1000;

    SinusGenerator generator;
    generator.setAmplitude(1);
    generator.setFrequency(100);
    generator.setSampleTime(sample_time_seconds);
    const float min_freq = 1;
    const float max_freq = 0.5/sample_time_seconds;
    const float freq_speed = (max_freq-min_freq)/100;
    float next_freq = generator.getFrequency() + freq_speed;
    bool direction = true;

    RectGenerator rectGenerator;
    rectGenerator.setAmplitude(1);
    rectGenerator.setFrequency(50);
    rectGenerator.setSampleTime(sample_time_seconds);

    const int dataPointsSize = 1500;
    std::queue<std::vector<DataPoint>> dataPoints;
    size_t dataPoints_sampled = 0;

    double time = 0;

    std::vector<std::complex<double>> fft_complex(dataPointsSize);
    const std::vector<double> fft_freq = linspace<double>(0.0, sample_frequency_Hz, dataPointsSize);

    httplib::Server svr;
    svr.Get("/data", [&fft_complex, &fft_freq](const httplib::Request &, httplib::Response &res){
        Json::Value json;
        json["fft"] = fromVector(fft_complex, fft_complex.size()/2 -1);
        json["freq"] = fromVector(fft_freq, fft_complex.size()/2 -1);
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
        {
            const auto freq = generator.getFrequency();
            if(freq >= max_freq){
                direction = false;
            }
            else if(freq <= min_freq){
                direction = true;
            }

            if(next_freq == freq){
                if(direction){
                    next_freq = freq + freq_speed;
                }
                else{
                    next_freq = freq - freq_speed;
                }
            }
            if(next_freq < min_freq){
                next_freq = min_freq;
            }
            else if(next_freq > max_freq){
                next_freq = max_freq;
            }
        }

        float signal = generator.next() + rectGenerator.next() + noiseGenerator.generate();
        data[dataPoints_sampled] = DataPoint(time, signal);
        dataPoints_sampled++;

        if(dataPoints_sampled == dataPointsSize){
            dataPoints.push(data);
            data = std::vector<DataPoint>(dataPointsSize);
            dataPoints_sampled = 0;
            generator.setFrequency(next_freq);
        }

        time += sample_time_seconds;
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(sample_time_seconds*1000)));
    }
    return 0;
}
