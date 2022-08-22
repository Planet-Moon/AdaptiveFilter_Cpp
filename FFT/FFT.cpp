#include "FFT.h"
#include <cmath>

const double PI = std::acos(-1);
using namespace std::complex_literals;

std::vector<std::complex<double>> FFT::dft(std::vector<double> const& input){
    const int n = input.size();
    std::vector<std::complex<double>> output(n, 0);
    #pragma omp parallel for
    for(int i = 0; i < n; ++i){
        std::complex<double> sum;
        #pragma omp parallel for
        for(int j = 0; j < n; ++j){
            std::complex<double> angle = 2i * static_cast<double>(PI * j * i / n);
            sum += input[j] * exp(-angle);
        }
        output[i] = sum;
    }
    return output;
}

std::vector<FFT::PlotData> FFT::plotable_dft(std::vector<std::complex<double>> const& dft_output, double const& sample_frequency)
{
    std::vector<PlotData> output(dft_output.size());
    const double frequency_step = (sample_frequency*0.5)/(dft_output.size()-1);
    #pragma omp parallel for
    for(int i = 0; i < dft_output.size(); ++i)
    {
        PlotData data{};
        data.freq = frequency_step * i;
        data.abs = std::abs(dft_output[i]);
        data.arg = std::arg(dft_output[i]);
        output[i] = data;
    }
    return output;
}

FFT::PlotDataFormatted FFT::PlotData::format() const
{
    PlotDataFormatted output;
    output.freq = freq;
    output.abs = 20 * log10(abs);
    output.arg = arg/PI;
    return output;
}
