#pragma once
#include <vector>
#include <string>

using Order = unsigned int;

class Fir{
public:
    Fir(const std::vector<double>& b);

    double filter(const double& input);

    std::string impz() const;

private:
    std::vector<double> b;
    std::vector<double> x_memory;
    Order n;
};
