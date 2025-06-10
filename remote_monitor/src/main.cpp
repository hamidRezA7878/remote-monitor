
#include "ssh_connection.h"
#include "hardware_monitor.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <user@host> <password> <cpu_threshold> <mem_threshold> [<port>]" << std::endl;
        return 1;
    }

    std::string user_host = argv[1];
    std::string password = argv[2];
    float cpuThreshold = std::stof(argv[3]);
    float memThreshold = std::stof(argv[4]);
    int port = argc > 5 ? std::stoi(argv[5]) : 22;

    auto at_pos = user_host.find('@');
    if (at_pos == std::string::npos) {
        std::cerr << "Invalid user@host format" << std::endl;
        return 1;
    }

    std::string user = user_host.substr(0, at_pos);
    std::string host = user_host.substr(at_pos + 1);

    monitorSystem(user, host, password, cpuThreshold, memThreshold, port);

    return 0;
}
