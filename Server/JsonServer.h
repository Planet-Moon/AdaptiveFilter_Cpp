#pragma once
#include <cstddef>
#include <json/json.h>
#include <httplib.h>
#include <vector>
#include <MatrixVector.h>


namespace MV = MatrixVector;

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

    template <size_t N>
    static Json::Value fromVector(const MV::Vec<N>& v){
        Json::Value result;
        for(int i = 0; i < N; i++){
            result[i] = v[i];
        }
        return result;
    }

    template <size_t Z, size_t S>
    static Json::Value fromMatrix(const MV::Mat<Z,S>& m){
        Json::Value result;
        for(int i = 0; i < Z; ++i){
            result[i] = fromVector(m[i]);
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
