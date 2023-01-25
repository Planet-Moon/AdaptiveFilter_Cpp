#include "Spectrogram.h"
#include <algorithm>
#include <iostream>
#include <numeric>

Spectrogram::Spectrogram(int _buffer_size): buffer_size(_buffer_size) {
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
        _last_datapoint_before_update.release();
        _last_datapoint_before_update = std::make_unique<DataPoint>(_buffer.back());
        _buffer.clear();
    }
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
    const double T_s = mean_sampletime/1'000'000'000; // s
    const double f_s = 1'000'000'000/mean_sampletime; // Hz

    std::vector<double> samples(datapoints.size());
    std::transform(datapoints.begin(), datapoints.end(), samples.begin(), [](const DataPoint& dp){
        return dp.value;
    });
    std::vector<std::complex<double>> result = FFT::fft(samples);

    std::cout << "T: " << T_s << std::endl;
    return result;
}
