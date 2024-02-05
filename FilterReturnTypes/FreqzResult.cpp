#include "FreqzResult.h"
#include <fstream>
#include <algorithm>
#include <json/json.h>


std::vector<std::string> FreqzResult::h_toStringVec() const
{
    std::vector<std::string> result(h.size());
    #pragma omp parallel for
    for(int i = 0; i < h.size(); ++i){
        const double real = h[i].real();
        const double imag = h[i].imag();
        std::string s = std::to_string(real);
        if(imag >= 0){
            s += "+";
        }
        else{
            s += "";
        }
        s += std::to_string(imag)+"i";
        result[i] = s;
    }
    return result;
}

int FreqzResult::size() const{
    return w.size();
}

std::vector<double> FreqzResult::magnitude() const {
    std::vector<double> result(h.size(),0);
    #pragma omp parallel for
    for(int i=0; i<h.size(); i++){
        result[i] = std::abs(h[i]);
    }
    return result;
}

std::vector<double> FreqzResult::phase() const {
    std::vector<double> result(h.size(),0);
    #pragma omp parallel for
    for(int i=0; i<h.size(); i++){
        result[i] = std::arg(h[i]);
    }
    return result;
}

void write_freqz_to_csv(const std::string &filename, const FreqzResult& freqz){
    std::string output = "";

    output += "Frequency;Magnitude;Phase\n";
    const auto mag = freqz.magnitude();
    const auto arg = freqz.phase();
    for(int i=0; i<freqz.w.size(); i++){
        output += std::to_string(freqz.w[i]) +";"+ std::to_string(mag[i]) +";"+ std::to_string(arg[i])+"\n";
    }

    std::replace( output.begin(), output.end(), '.', ',' );
    std::ofstream file;
    file.open(filename);
    file << output;
    file.close();
}

void write_freqz_to_json(const std::string &filename, const FreqzResult& freqz){
    const auto mag = freqz.magnitude();
    const auto arg = freqz.phase();

    Json::Value json;
    json["Frequency"] = Json::arrayValue;
    for(int i=0; i<freqz.w.size(); i++){
        json["Frequency"][i] = freqz.w[i];
    }

    json["Magnitude"] = Json::arrayValue;
    for(int i=0; i<freqz.w.size(); i++){
        json["Magnitude"][i] = mag[i];
    }

    json["Phase"] = Json::arrayValue;
    for(int i=0; i<freqz.w.size(); i++){
        json["Phase"][i] = arg[i];
    }

    std::ofstream file_id;
    file_id.open(filename);
    Json::StyledWriter styledWriter;
    file_id << styledWriter.write(json);
    file_id.close();
}
