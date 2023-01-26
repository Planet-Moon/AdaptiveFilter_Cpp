#include "RectGenerator.h"

RectGenerator::RectGenerator() : SinusGenerator() {

}

RectGenerator::~RectGenerator(){

}

float RectGenerator::next() {
    float sin_val = SinusGenerator::next();
    if(sin_val > 0){
        return getAmplitude();
    }
    else if (sin_val < 0)
    {
        return -1 * getAmplitude();
    }
    else{
        return 0;
    }
}
