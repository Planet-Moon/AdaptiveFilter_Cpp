#pragma once
#include "DataSource.h"
#include <thread>

class CLI_DataSource : public DataSource {
public:
    CLI_DataSource();
    virtual ~CLI_DataSource();

    Json::Value get_data() override;

    void collect_cli_lines();

private:
    std::thread _cli_thread;
    bool _run_cli = true;
    std::vector<std::string> _cli_lines;
};
