#include <thread>
#include "SinusGenerator.h"
#include "WhiteNoise.h"
#include "FFT.h"
#include "DataPoint.h"
#include <httplib.h>

static void server_thread_func(httplib::Server& server, const char* address, int port){
    server.listen(address, port);
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
static Json::Value fromVector(std::vector<std::complex<T>> vector){
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


int main(int argc, const char** argv) {
    WhiteNoise noiseGenerator(0, 0.5);

    const double sample_time_seconds = 0.001;

    SinusGenerator generator;
    generator.setAmplitude(1);
    generator.setFrequency(250);
    generator.setSampleTime(sample_time_seconds);

    const int dataPointsSize = 1000;
    std::vector<DataPoint> dataPoints(dataPointsSize);
    uint16_t dataPoints_sampled = 0;

    double time = 0;

    std::vector<std::complex<double>> fft_complex(dataPointsSize);
    std::vector<double> fft_freq(dataPointsSize);

    httplib::Server svr;
    svr.Get("/data", [&fft_complex, &fft_freq](const httplib::Request &, httplib::Response &res){
        Json::Value json;
        json["fft"] = fromVector(fft_complex);
        json["freq"] = fromVector(fft_freq);
        std::string content = convert_to_string(json);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(content, "application/json");
    });
    auto server_thread = std::make_unique<std::thread>(server_thread_func, svr, "0.0.0.0", 8801);

    while (true)
    {
        dataPoints[dataPoints_sampled] = DataPoint(time, generator.next());
        dataPoints_sampled++;

        if(dataPoints_sampled == dataPointsSize){
            fft_complex = FFT::fft(DataPoint::toVec<double>(dataPoints));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long>(sample_time_seconds*1000)));
    }
    return 0;
}
