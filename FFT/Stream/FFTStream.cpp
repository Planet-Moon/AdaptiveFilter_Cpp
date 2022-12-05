#include <FFT.h>
#include "FFTStream.h"
#include "HTTP_DataSource.h"

std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
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

void server_thread_func(std::shared_ptr<httplib::Server> server, const char* address, int port){
    server->listen(address, port);
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

template <typename T>
static Json::Value fromVector(std::vector<T> vector){
    Json::Value result;
    for(int i = 0; i < vector.size(); i++){
        result[i] = std::to_string(vector[i]);
    }
    return result;
}

float calc_sample_time(const std::vector<DataPoint>& dp){
    float _sample_time = 0.0;
    for(int i = 1; i < dp.size(); i++){
        _sample_time += ((dp[i].time - dp[i-1].time) - _sample_time)/i;
    }
    return _sample_time;
}

FFTStream::FFTStream(){
    // Data storage
    dataSource = std::make_unique<HTTP_DataSource>("http://127.0.0.1:8800","/data");
    dataPoints = std::vector<DataPoint>(dataPoints_maxSize);

    svr = std::make_shared<httplib::Server>();
    fft_result = std::make_shared<std::vector<std::complex<double>>>();
    fft_freq_result = std::make_shared<std::vector<double>>();
    svr->Get("/data", [this](const httplib::Request &, httplib::Response &res){
        Json::Value json;
        json["fft"] = fromVector(*fft_result);
        json["freq"] = fromVector(*fft_freq_result);
        std::string content = convert_to_string(json);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(content, "application/json");
    });
    server_thread = std::make_unique<std::thread>(server_thread_func, svr, "0.0.0.0", 8801);
}

void FFTStream::run(){
    while(_run){
        {   // Get new data
            Json::Value res = dataSource->get_data();
            std::vector<DataPoint> new_points = DataPoint::fromJson(res);
            size_t remaining = dataPoints_maxSize - dataPoint_size;
            size_t to_copy = std::min(new_points.size(), remaining);
            std::copy_n(new_points.end()-to_copy, to_copy, dataPoints.begin()+dataPoint_size);
            dataPoint_size += new_points.size();
        }

        if(dataPoint_size > dataPoints_maxSize){
            std::vector<DataPoint> fft_vec(dataPoints.begin(), dataPoints.end());
            { // reset data points
                dataPoints = std::vector<DataPoint>(dataPoints_maxSize, DataPoint());
                dataPoint_size = 0;
            }

            // Calculate fast fourier transform
            *fft_result = FFT::fft(DataPoint::toVec<double>(fft_vec));

            { // Calculate vector of frequencies
                float chunk_sample_time = calc_sample_time(fft_vec);
                sample_time = sample_time + (chunk_sample_time - sample_time)/samples_chunks_got;
                ++samples_chunks_got;
                float sample_freq = 1/sample_time;
                sample_freq = 1000; //! DEBUG
                fft_freq_result->clear();
                *fft_freq_result = linspace<double>(0.f, sample_freq, fft_result->size());
            }

            { // find dominant frequency
                struct{
                    int idx=0;
                    double mag=0;
                    double freq=0;
                } dominant_freq_Hz_idx;

                // calculate magnitudes of fft points
                std::vector<double> fft_result_abs(fft_result->size());
                std::transform(fft_result->begin(), fft_result->end(), fft_result_abs.begin(),
                    [](const std::complex<double>& val){
                        return  std::abs(val);
                });

                // find largest magnitude
                for(int i = 0; i < fft_result_abs.size()*0.5; ++i){
                    if(fft_result_abs[i] > dominant_freq_Hz_idx.mag){
                        dominant_freq_Hz_idx.mag = fft_result_abs[i];
                        dominant_freq_Hz_idx.idx = i;
                    }
                }

                // get frequency of the index with the largest magnitude
                dominant_freq_Hz_idx.freq = (*fft_freq_result)[dominant_freq_Hz_idx.idx];
                std::cout << " dominant frequency: " << dominant_freq_Hz_idx.freq << " Hz" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void FFTStream::stop() {
    _run = false;
}
