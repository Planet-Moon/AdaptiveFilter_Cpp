#pragma once
#include <chrono>
#include <queue>
#include <thread>
#include <FFT.h>


class Spectrogram{
public:
    Spectrogram(unsigned int buffer_size = 1024, unsigned int display_buffer_size = 128);
    ~Spectrogram();
    void addSample(double sample);

    const unsigned int buffer_size;
    unsigned int display_buffer_size;

    std::vector<std::vector<std::complex<double>>> display_buffer;

    void setEvaluatedSamples(unsigned int value);
    unsigned int getEvaluatedSamples() const;

    double sampleFrequency() const;
    double bufferTime() const;

    static std::vector<double> abs(const std::vector<std::complex<double>>& vector);
    static std::vector<double> arg(const std::vector<std::complex<double>>& vector);

    static std::vector<double> toDecibel(const std::vector<double>& vector);
    static std::vector<double> fromDecibel(const std::vector<double>& vector);

private:
    unsigned int _sample_counter = 0;
    unsigned int _sample_counter_max = 10;
    mutable double _sample_frequency = -1;
    mutable double _buffer_time = -1;
    struct DataPoint{
        double value;
        std::chrono::steady_clock::time_point timepoint;
    };
    std::vector<DataPoint> _buffer{};

    std::thread fft_thread;
    bool fft_thread_run = true;
    std::queue<std::vector<DataPoint>> fft_input_queue;
    std::queue<std::vector<std::complex<double>>> fft_output_queue;

    void _evaluateTimepoints(const std::vector<DataPoint>& datapoints);
    static std::vector<std::complex<double>> calculate_fft(const std::vector<DataPoint>& datapoints);

    /**
     * @brief Calculate mean delta t beween timepoints
     *
     * @param timepoints
     * @return double nanoseconds
     */
    double _mean_sampletime(const std::vector<std::chrono::steady_clock::time_point>& timepoints) const;
};
