#include "FilterReturnTypes.h"

Json::Value UpdateStats::toJson() const
{
    Json::Value result;
    result["error"] = error;
    Json::Value bJson{};
    for(int i=0; i<b.size(); ++i){
        bJson[i] = b[i];
    }
    result["b"] = bJson;
    result["y"] = y;
    return result;
}

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
