#include "CLI_DataSource.h"
#include <iostream>

CLI_DataSource::CLI_DataSource():
    DataSource()
{
    _cli_thread = std::thread(&CLI_DataSource::collect_cli_lines, this);
}

CLI_DataSource::~CLI_DataSource(){
    _run_cli = false;
    _cli_thread.join();
};

Json::Value CLI_DataSource::get_data() {
    Json::Value result = parseString(*_cli_lines.begin());
    _cli_lines.erase(_cli_lines.begin());
    return result;
};

void CLI_DataSource::collect_cli_lines(){
    std::string line;
    while (_run_cli && std::getline(std::cin, line)) {
        _cli_lines.push_back(line);
    }
}
