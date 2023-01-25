#include "Spectrogram.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <cassert>

template<typename T>
static void rotate_vector(std::vector<T>& vector){
    if(vector.size() > 1)
        std::rotate(vector.rbegin(), vector.rbegin()+1, vector.rend());
    vector.front() = T();
}

template<typename T>
static void rotate_vector(std::vector<T>& vector, const T& front_data){
    if(vector.size() > 1)
        std::rotate(vector.rbegin(), vector.rbegin()+1, vector.rend());
    vector.front() = front_data;
}

Spectrogram::Spectrogram(unsigned int _buffer_size, unsigned int _display_buffer_size):
    buffer_size(_buffer_size), display_buffer_size(_display_buffer_size)
{
    _buffer = std::vector<DataPoint>(buffer_size);
    const auto helper = std::vector<std::complex<double>>(buffer_size);
    display_buffer = std::vector<std::vector<std::complex<double>>>(display_buffer_size, helper);

    fft_thread = std::thread(
        [](const bool& run, std::queue<std::vector<DataPoint>>& input, std::queue<std::vector<std::complex<double>>>& output)
        {
            std::vector<Spectrogram::DataPoint> i;
            while(run){
                if(input.empty())
                    continue;
                i = input.front();
                input.pop();
                output.emplace(Spectrogram::calculate_fft(i));
            }
        },
        std::ref(fft_thread_run), std::ref(fft_input_queue), std::ref(fft_output_queue)
    );
}

Spectrogram::~Spectrogram(){
    fft_thread_run = false;
    fft_thread.join();
}

void Spectrogram::addSample(double sample)
{
    DataPoint dp;
    dp.timepoint = std::chrono::steady_clock::now();
    dp.value = sample;
    rotate_vector(_buffer, dp);
    _sample_counter++;
    if(_sample_counter >= _sample_counter_max){
        _evaluateTimepoints(_buffer);
        fft_input_queue.push(_buffer);
        rotate_vector(display_buffer);
        _sample_counter = 0;
    }
}

void Spectrogram::setEvaluatedSamples(unsigned int value) {
    assert(value <= _buffer.size()); // value should not be smaller than buffer size
    _sample_counter_max = value;
}

unsigned int Spectrogram::getEvaluatedSamples() const {
    return _sample_counter_max;
}

double Spectrogram::sampleFrequency() const {
    return _sample_frequency;
}

double Spectrogram::bufferTime() const {
    return _buffer_time;
}

std::vector<double> Spectrogram::abs(const std::vector<std::complex<double>>& vector){
    std::vector<double> result(vector.size());
    std::transform(vector.begin(), vector.end(), result.begin(), [](const std::complex<double>& v){
        return std::abs(v);
    });
    return result;
}

std::vector<double> Spectrogram::arg(const std::vector<std::complex<double>>& vector){
    std::vector<double> result(vector.size());
    std::transform(vector.begin(), vector.end(), result.begin(), [](const std::complex<double>& v){
        return std::arg(v);
    });
    return result;
}

std::vector<double> Spectrogram::toDecibel(const std::vector<double>& vector){
    std::vector<double> result(vector.size());
    std::transform(vector.begin(), vector.end(), result.begin(), [](double v){
        return 20*std::log10(v);
    });
    return result;
}

std::vector<double> Spectrogram::fromDecibel(const std::vector<double>& vector){
    std::vector<double> result(vector.size());
    std::transform(vector.begin(), vector.end(), result.begin(), [](double v){
        return std::pow(10,v/20);
    });
    return result;
}

double Spectrogram::_mean_sampletime(const std::vector<std::chrono::steady_clock::time_point>& timepoints) const {
    std::vector<std::chrono::duration<long, std::nano>> time_delta(timepoints.size()-1);
    const int divider = timepoints.size()-1;
    double sum = 0.;
    for(size_t i = 0; i < timepoints.size()-1; ++i){
        sum += (timepoints[i] - timepoints[i+1]).count(); // nanoseconds
    }
    double mean = sum / divider;
    return mean;
}

void Spectrogram::_evaluateTimepoints(const std::vector<DataPoint>& datapoints){
    std::vector<std::chrono::steady_clock::time_point> timepoints(datapoints.size());
    std::transform(datapoints.begin(), datapoints.end(), timepoints.begin(), [](const DataPoint& dp){
        return dp.timepoint;
    });
    const double mean_sampletime = _mean_sampletime(timepoints);
    const double f_s = 1'000'000'000/mean_sampletime; // Hz
    _sample_frequency = f_s;
    _buffer_time = buffer_size * mean_sampletime/1'000'000'000;
}

std::vector<std::complex<double>> Spectrogram::calculate_fft(const std::vector<DataPoint>& datapoints){
    std::vector<double> samples(datapoints.size());
    std::transform(datapoints.begin(), datapoints.end(), samples.begin(), [](const DataPoint& dp){
        return dp.value;
    });
    std::vector<std::complex<double>> result = FFT::fft(samples);
    return result;
}
