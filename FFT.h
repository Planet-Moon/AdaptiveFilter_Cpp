#pragma once
#include <complex>
#include <vector>
#include <map>

class FFT{
public:
    static std::vector<std::complex<double>> dft(std::vector<double> const& input);

    struct PlotDataFormatted{
        double freq;
        double abs;
        double arg;
    };

    struct PlotData{
        double freq;
        double abs;
        double arg;
        PlotDataFormatted format() const;
    };

    static std::vector<PlotData> plotable_dft(std::vector<std::complex<double>> const& dft_output, double const& sample_frequency);
};
