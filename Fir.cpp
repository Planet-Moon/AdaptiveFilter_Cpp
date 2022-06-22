#include "Fir.h"
#include <algorithm>

Fir::Fir(const std::vector<double>& b): b(b), n(b.size())
{
    x_memory = std::vector<double>(n,0);
}

double Fir::filter(const double& input)
{
    if(x_memory.size() > 1)
        std::rotate(x_memory.rbegin(), x_memory.rbegin()+1, x_memory.rend());
    x_memory[0] = input;

    double y = 0.0;
    for(unsigned int i = 0; i < n; i++){
        y += x_memory[i] * b[i];
    }
    return y;
}

std::string Fir::impz() const
{
    std::string result = "";
    for(unsigned int i = 0; i < n; ++i){
        if(b[i] == 0) continue;
        result += std::to_string(b[i]) + "*z^" + std::to_string(i);
        if(i+1 < n && b[i+1] >= 0)
            result += " + ";
    }
    return result;
}
