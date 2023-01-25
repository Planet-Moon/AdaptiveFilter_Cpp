#include "Spectrogram.h"
#include <algorithm>
#include <iostream>
#include <numeric>

template<typename T>
static void rotate_vector(std::vector<T>& vector){
    if(vector.size() > 1)
        std::rotate(vector.rbegin(), vector.rbegin()+1, vector.rend());
    vector.front() = T();
}

Spectrogram::Spectrogram(unsigned int _buffer_size, unsigned int _display_buffer_size):
    buffer_size(_buffer_size), display_buffer_size(_display_buffer_size) {
        const auto helper = std::vector<std::complex<double>>(buffer_size);
        display_buffer = std::vector<std::vector<std::complex<double>>>(display_buffer_size, helper);
}

Spectrogram::~Spectrogram(){

}

void Spectrogram::addSample(double sample)
{
    DataPoint dp;
    dp.timepoint = std::chrono::steady_clock::now();
    dp.value = sample;
    _buffer.push_back(dp);

    if(_buffer.size() >= buffer_size){
        const auto fft_complex_result = _calculate_fft(_buffer);
        rotate_vector(display_buffer);
        display_buffer.front() = fft_complex_result;

        _last_datapoint_before_update.release();
        _last_datapoint_before_update = std::make_unique<DataPoint>(_buffer.back());
        _buffer.clear();
    }
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

std::vector<Spectrogram::Color> Spectrogram::vector2Color(const std::vector<double>& vector) const {
    return {}; // todo
}

double Spectrogram::_mean_sampletime(const std::vector<std::chrono::steady_clock::time_point>& timepoints) const {
    std::vector<std::chrono::duration<long, std::nano>> time_delta(timepoints.size()-1);
    double sum = 0.;
    const int divider = _last_datapoint_before_update ? timepoints.size() : timepoints.size()-1;
    if(_last_datapoint_before_update){
        sum += (_last_datapoint_before_update->timepoint - timepoints[0]).count();
    }
    for(size_t i = 0; i < timepoints.size()-1; ++i){
        sum += (timepoints[i+1] - timepoints[i]).count(); // nanoseconds
    }
    double mean = sum / divider;
    return mean;
}

std::vector<std::complex<double>> Spectrogram::_calculate_fft(const std::vector<DataPoint>& datapoints) const {
    std::vector<std::chrono::steady_clock::time_point> timepoints(datapoints.size());
    std::transform(datapoints.begin(), datapoints.end(), timepoints.begin(), [](const DataPoint& dp){
        return dp.timepoint;
    });
    const double mean_sampletime = _mean_sampletime(timepoints);
    const double f_s = 1'000'000'000/mean_sampletime; // Hz
    _sample_frequency = f_s;
    _buffer_time = buffer_size * mean_sampletime/1'000'000'000;

    std::vector<double> samples(datapoints.size());
    std::transform(datapoints.begin(), datapoints.end(), samples.begin(), [](const DataPoint& dp){
        return dp.value;
    });
    std::vector<std::complex<double>> result = FFT::fft(samples);
    return result;
}
