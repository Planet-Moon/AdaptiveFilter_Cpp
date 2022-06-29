#pragma once
#include <vector>
#include <string>

using Order = unsigned int;

class Fir{
public:
    Fir(const std::vector<double>& b);

    double filter(const double& input);

    std::string impz() const;

    std::vector<double> b() const;
    Order n() const;

private:
    std::vector<double> _b;
    std::vector<double> x_memory;
    Order _n;
};
