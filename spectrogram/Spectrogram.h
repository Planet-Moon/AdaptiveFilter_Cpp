#pragma once
#include <chrono>
#include <queue>
#include <FFT.h>


class Spectrogram{
public:
    Spectrogram(unsigned int buffer_size = 1024, unsigned int display_buffer_size = 128);
    ~Spectrogram();
    void addSample(double sample);

    const unsigned int buffer_size;
    unsigned int display_buffer_size;

    std::vector<std::vector<std::complex<double>>> display_buffer;

    double sampleFrequency() const;
    double bufferTime() const;

    static std::vector<double> abs(const std::vector<std::complex<double>>& vector);
    static std::vector<double> arg(const std::vector<std::complex<double>>& vector);

    static std::vector<double> toDecibel(const std::vector<double>& vector);
    static std::vector<double> fromDecibel(const std::vector<double>& vector);

    struct Color{
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };
    std::vector<Color> vector2Color(const std::vector<double>& vector) const;

    unsigned int minimum_abs = 0;
    unsigned int maximum_abs = 100;

private:

    mutable double _sample_frequency = -1;
    mutable double _buffer_time = -1;
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
