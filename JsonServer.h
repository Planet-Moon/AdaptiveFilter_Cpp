#pragma once
#include <json/json.h>
#include <httplib.h>
#include <vector>

class JsonServer{
public:
    JsonServer(int port, const Json::Value& json);
    ~JsonServer();
    bool is_running() const;
    static void host(httplib::Server* svr, int port);
    void host_blocking(bool once = true);
    int sleep_period = 100; // ms

    template <typename T>
    static Json::Value fromVector(std::vector<T> vector){
        Json::Value result;
        for(int i = 0; i < vector.size(); i++){
            result[i] = vector[i];
        }
        return result;
    }

    template <typename T>
    static Json::Value fromMatrix(std::vector<std::vector<T>> vector){
        Json::Value result;
        for(int i = 0; i < vector.size(); i++){
            Json::Value value;
            for(int j = 0; j < vector[i].size(); j++){
                value[j] = vector[i][j];
            }
            result[i] = value;
        }
        return result;
    }

    std::thread* thread();

private:
    httplib::Server svr;
    int port = 80;
    Json::Value json;
    int requests_gotten = 0;
    std::thread server_thread;
};
