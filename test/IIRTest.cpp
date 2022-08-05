#include "gtest/gtest.h"
#include <Iir.h>
#include <fstream>
#include <WhiteNoise.h>
#include <FFT.h>
#include <json/json.h>

void write_to_file(std::vector<double> const& v1, std::vector<double> const& v2, std::vector<FFT::PlotData> const& spec_v1, std::vector<FFT::PlotData> const& spec_v2){
    std::string output = "";
    output += "Vector1;Vector2;Freq;Abs;Arg;Freq_f;Abs_f;Arg_f\n";
    for(int i = 0; i < v1.size(); i++){
        output += std::to_string(v1[i]) + ";" + std::to_string(v2[i]);
        if(i < spec_v1.size()){
            output += ";";
            const auto& _s = spec_v1[i];
            const auto s = _s.format();
            output += std::to_string(s.freq) + ";" + std::to_string(s.abs) + ";" + std::to_string(s.arg);

        }
        if(i < spec_v2.size()){
            output += ";";
            const auto& _s = spec_v2[i];
            const auto s = _s.format();
            output += std::to_string(s.freq) + ";" + std::to_string(s.abs) + ";" + std::to_string(s.arg);
        }
        output += "\n";
    }
    std::replace( output.begin(), output.end(), '.', ',' );

    std::ofstream file;
    file.open("iir_test.csv");
    file << output;
    file.close();
}

void write_json(const Json::Value& json){
    std::ofstream file_id;
    file_id.open("data.json");
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(json);
    file_id.close();
}

TEST(IIR, Filter){
    WhiteNoise whiteNoise(1, 0.3);

    std::vector<double> a{1, 0.2};
    std::vector<double> b(5, 0.2);
    IIR iir(a, b);

    const int samples = 1e4;
    std::vector<double> result(samples, 0);
    std::vector<double> input(samples, 0);
    for(int i=0; i<samples; i++){
        const double noise = whiteNoise.generate();
        input[i] = noise;
        const double output = iir.filter(noise);
        result[i] = output;
    }

    const auto dft_unfiltered = FFT::dft(input);
    const auto dft_filtered = FFT::dft(result);

    const auto unfiltered = FFT::plotable_dft(dft_unfiltered, 200);
    const auto filtered = FFT::plotable_dft(dft_filtered, 200);

    write_to_file(input, result, unfiltered, filtered);
}

TEST(IIR, freqz){
    std::vector<double> a{1, 0.2};
    std::vector<double> b(5, 0.2);
    IIR iir(a, b);

    const FreqzResult freqz = iir.freqz(50);
    write_freqz_to_csv("freqz_result.csv", freqz);
    write_freqz_to_json("freqz_result.json", freqz);
    const auto mag = freqz.magnitude();
    const auto arg = freqz.phase();
}
