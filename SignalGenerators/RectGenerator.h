#pragma once
#include "SinusGenerator.h"

class RectGenerator : public SinusGenerator {
public:
    RectGenerator();
    virtual ~RectGenerator();

    virtual float next() override;
protected:
private:
};
