#pragma once
#include <memory>
#include <vector>
#include <complex>
#include "DataPoint.h"
#include "DataSource.h"
#include <httplib.h>

class FFTStream{
public:
    FFTStream();
    void run();
    void stop();
private:
    std::unique_ptr<DataSource> dataSource;
    const size_t dataPoints_maxSize = 1000;
    std::vector<DataPoint> dataPoints;
    size_t dataPoint_size = 0;

    // Server for python gui
    std::shared_ptr<httplib::Server> svr;
    std::shared_ptr<std::vector<std::complex<double>>> fft_result;
    std::shared_ptr<std::vector<double>> fft_freq_result;

    float sample_time = 0;
    long samples_chunks_got = 1;

    std::unique_ptr<std::thread> server_thread;

    bool _run = true;
};
