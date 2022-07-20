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
    std::vector<std::string> result{};
    for(const auto& h_e: h){
        const double real = h_e.real();
        const double imag = h_e.imag();
        std::string s = std::to_string(real);
        if(imag >= 0){
            s += "+";
        }
        else{
            s += "";
        }
        s += std::to_string(imag)+"i";
        result.push_back(s);
    }
    return result;
}

int FreqzResult::size() const{
    return w.size();
}
