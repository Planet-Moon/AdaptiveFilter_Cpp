#pragma once
#include <vector>
#include <FreqzResult.h>

class IIR{
public:
    IIR(std::vector<double>& a, std::vector<double>& b);

    double filter(const double& x);

    enum Form{
        DF1,
        DF2
    };

    void set_form(const Form& f);
    Form get_form() const;

    FreqzResult freqz(int samples /* = 50 */) const;
protected:
private:
    void update_x(const double& x);
    void update_y(const double& y);
    std::vector<double> a;
    std::vector<double> b;
    std::vector<double> x;
    std::vector<double> y;
    Form form;
};
