#include "UpdateStats.h"


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
