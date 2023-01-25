#pragma once
#include <chrono>
#include <FFT.h>


class Spectrogram{
public:
    Spectrogram(int buffer_size = 0);
    ~Spectrogram();
    void addSample(double sample);

    unsigned int buffer_size = 1024;

private:
    struct DataPoint{
        double value;
        std::chrono::steady_clock::time_point timepoint;
    };
    std::unique_ptr<DataPoint> _last_datapoint_before_update{};
    std::vector<DataPoint> _buffer{};


    std::vector<std::complex<double>> _calculate_fft(const std::vector<DataPoint>& datapoints) const;

    /**
     * @brief Calculate mean delta t beween timepoints
     *
     * @param timepoints
     * @return double nanoseconds
     */
    double _mean_sampletime(const std::vector<std::chrono::steady_clock::time_point>& timepoints) const;
};
