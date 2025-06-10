
#include "ssh_connection.h"
#include <cstdlib>
#include <sstream>

bool establishSSHConnection(const std::string& user, const std::string& host, const std::string& password, int port) {
    std::ostringstream cmd;
    cmd << "sshpass -p " << password
        << " ssh -o StrictHostKeyChecking=no -p " << port
        << " " << user << "@" << host << " echo SSH connection established";
    return system(cmd.str().c_str()) == 0;
}

std::string runRemoteCommand(const std::string& user, const std::string& host, const std::string& password, const std::string& command, int port) {
    std::ostringstream cmd;
    cmd << "sshpass -p " << password
        << " ssh -o StrictHostKeyChecking=no -p " << port
        << " " << user << "@" << host << " "" << command << """;
    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return "ERROR";

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}
