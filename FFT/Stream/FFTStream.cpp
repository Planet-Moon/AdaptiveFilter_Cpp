#include <csignal>
#include <string>
#include <iostream>
#include <thread>
#include <json/json.h>
#include <httplib.h>
#include <FFT.h>

bool running = true;

void signalHandler(int signum) {
    switch (signum) {
        case SIGINT:
            running = false;
            break;
        default:
            break;
    }
}

Json::Value parseString(const std::string& str_, const Json::CharReaderBuilder& _builder){
    Json::Value result;
    Json::String errs;
    std::stringstream ss;
    ss.str(str_);
    bool ok = Json::parseFromStream(_builder, ss, &result, &errs);
    return result;
}

std::string convert_to_string(const Json::Value& json){
    return Json::FastWriter().write(json);
}

class DataSource{
public:
    DataSource(){
        _builder["collectComments"] = false;
    };
    virtual ~DataSource() = default;

    virtual Json::Value get_data() = 0;

protected:
    Json::CharReaderBuilder _builder;

    Json::Value parseString(const std::string& str_){
        return ::parseString(str_, _builder);
    }

private:
};

class CLI_DataSource : public DataSource {
public:
    CLI_DataSource():
        DataSource()
    {
        _cli_thread = std::thread(&CLI_DataSource::collect_cli_lines, this);
    }

    virtual ~CLI_DataSource(){
        _run_cli = false;
        _cli_thread.join();
    };

    Json::Value get_data() override {
        Json::Value result = parseString(*_cli_lines.begin());
        _cli_lines.erase(_cli_lines.begin());
        return result;
    };

    void collect_cli_lines(){
        std::string line;
        while (_run_cli && std::getline(std::cin, line)) {
            _cli_lines.push_back(line);
        }
    }

private:
    std::thread _cli_thread;
    bool _run_cli = true;
    std::vector<std::string> _cli_lines;
};

class HTTP_DataSource : public DataSource {
public:
    HTTP_DataSource(const std::string& host, const std::string& url):
        DataSource()
    {
        _cli = std::make_unique<httplib::Client>(host);
        _url = url;
    }

    virtual ~HTTP_DataSource(){

    }

    Json::Value get_data() override {
        auto res = _cli->Get(_url.c_str());
        Json::Value result = parseString(res->body);
        return result;
    };

private:
    std::unique_ptr<httplib::Client> _cli;
    std::string _url;
};

struct DataPoint{
    DataPoint(const float& t, const float& v): time(t), value(v) {};
    float time;
    float value;

    static std::vector<DataPoint> fromJson(const Json::Value& jv){
        const Json::Value& _time = jv["time"];
        const Json::Value& _value = jv["value"];
        Json::ArrayIndex size = _time.size();
        std::vector<DataPoint> result;
        for(int i = 0; i < size; ++i){
            DataPoint p(_time[i].asFloat(), _value[i].asFloat());
            result.push_back(p);
        }
        // time = jv.get("time",0).asFloat();
        // value = jv.get("value",0).asFloat();
        return result;
    }

    template<typename T>
    static std::vector<T> toVec(const std::vector<DataPoint>& v){
        std::vector<T> result;
        for(const auto& i : v){
            result.push_back(static_cast<T>(i.value));
        }
        return result;
    }
};

void server_thread_func(std::shared_ptr<httplib::Server> server, const char* address, int port){
    server->listen(address, port);
}

template <typename T>
static Json::Value fromVector(std::vector<std::complex<T>> vector){
    Json::Value result;
    for(int i = 0; i < vector.size(); i++){
        std::string s = std::to_string(vector[i].real());
        const T& imag = vector[i].imag();
        if(imag < 0){
            s += std::to_string(imag)+"i";
        }
        else{
            s += "+"+std::to_string(imag)+"i";
        }
        result[i] = s;
    }
    return result;
}

template <typename T>
static Json::Value fromVector(std::vector<T> vector){
    Json::Value result;
    for(int i = 0; i < vector.size(); i++){
        result[i] = std::to_string(vector[i]);
    }
    return result;
}

float calc_sample_time(const std::vector<DataPoint>& dp){
    float _sample_time = 0.0;
    for(int i = 1; i < dp.size(); i++){
        _sample_time += ((dp[i].time - dp[i-1].time) - _sample_time)/i;
    }
    return _sample_time;
}

int main(int argc, char **argv){
    auto dataSource = std::make_unique<HTTP_DataSource>("http://127.0.0.1:8800","/data");
    std::vector<DataPoint> dataPoints;

    std::shared_ptr<httplib::Server> svr = std::make_shared<httplib::Server>();
    auto fft_result = std::make_shared<std::vector<std::complex<double>>>();
    auto fft_freq_result = std::make_shared<std::vector<double>>();
    svr->Get("/data", [&fft_result, &fft_freq_result](const httplib::Request &, httplib::Response &res){
        Json::Value json;
        json["fft"] = fromVector(*fft_result);
        json["freq"] = fromVector(*fft_freq_result);
        std::string content = convert_to_string(json);
        res.set_content(content, "application/json");
    });
    std::thread server_thread(server_thread_func, svr, "0.0.0.0", 8801);
    while(true){
        auto res = dataSource->get_data();
        auto new_points = DataPoint::fromJson(res);
        dataPoints.insert(dataPoints.end(), new_points.begin(), new_points.end());

        if(dataPoints.size() > 100){
            std::vector<DataPoint> fft_vec(dataPoints.end() - 100, dataPoints.end());
            float sample_time = calc_sample_time(fft_vec);
            float sample_freq = 1/sample_time;
            *fft_result = FFT::fft(DataPoint::toVec<double>(fft_vec));
            fft_freq_result->clear();
            for(int i = 0; i < fft_result->size(); ++i){
                fft_freq_result->push_back(i*sample_freq/fft_result->size());
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}
