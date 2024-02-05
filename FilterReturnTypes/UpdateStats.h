#pragma once
#include "Matrix.h"
#include "json/json.h"

struct UpdateStats{
    double error;
    double y;
    Vec b;
    Json::Value toJson() const;
};
