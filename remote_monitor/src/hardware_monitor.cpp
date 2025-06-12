#include "hardware_monitor.h"
#include "ssh_connection.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>

void monitorSystem(const std::string& user, const std::string& host, const std::string& password, float cpuThreshold, float memThreshold, int port) {
    if (!establishSSHConnection(user, host, password, port)) {
        std::cerr << "SSH connection failed" << std::endl;
        return;
    }
    std::cout << "Connected to " << host << " via SSH." << std::endl;

    while (true) {
        std::string cpuUsageStr = runRemoteCommand(user, host, password,
            "top -bn1 | grep '%Cpu' | awk '{print $2 + $4}'", port);

        std::string memUsageStr = runRemoteCommand(user, host, password,
            "free | grep Mem | awk '{print $3/$2 * 100.0}'", port);

        std::cout << "Raw CPU: [" << cpuUsageStr << "]" << std::endl;
        std::cout << "Raw MEM: [" << memUsageStr << "]" << std::endl;

        try {
            if (cpuUsageStr.empty() || memUsageStr.empty()) {
                std::cerr << "One of the usage values is empty. Skipping this cycle." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(30));
                continue;
            }

            float cpuUsage = std::stof(cpuUsageStr);
            float memUsage = std::stof(memUsageStr);

            std::cout << "[CPU Usage] " << cpuUsage << "% [Memory Usage] " << memUsage << "%" << std::endl;

            if (cpuUsage > cpuThreshold) {
                std::cout << "⚠️  CPU usage exceeds threshold!" << std::endl;
                std::string pid = runRemoteCommand(user, host, password,
                    "ps -eo pid,%cpu --sort=-%cpu | head -n 2 | tail -n 1 | awk '{print $1}'", port);
                std::cout << "Terminating process PID: " << pid << std::endl;
                runRemoteCommand(user, host, password, "kill -9 " + pid, port);
            }

            if (memUsage > memThreshold) {
                std::cout << "⚠️  Memory usage exceeds threshold!" << std::endl;
                std::string pid = runRemoteCommand(user, host, password,
                    "ps -eo pid,%mem --sort=-%mem | head -n 2 | tail -n 1 | awk '{print $1}'", port);
                std::cout << "Terminating process PID: " << pid << std::endl;
                runRemoteCommand(user, host, password, "kill -9 " + pid, port);
            }

        } catch (const std::exception& e) {
            std::cerr << "Error parsing usage values: " << e.what() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}
