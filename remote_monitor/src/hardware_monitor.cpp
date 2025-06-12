#include "hardware_monitor.h"
#include "ssh_connection.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <algorithm>

void monitorSystem(const std::string& user, const std::string& host, const std::string& password, float cpuThreshold, float memThreshold, int port) {
    if (!establishSSHConnection(user, host, password, port)) {
        std::cerr << "SSH connection failed" << std::endl;
        return;
    }
    std::cout << "Connected to " << host << " via SSH." << std::endl;

    while (true) {
        std::string cpuUsageStr = runRemoteCommand(user, host, password,
            "top -bn1 | grep 'Cpu(s)' | awk '{print 100 - $8}'", port);
        std::string memUsageStr = runRemoteCommand(user, host, password,
            "free | grep Mem | awk '{print ($3 / $2) * 100.0}'", port);

        // پاک‌سازی \n و فضاهای اضافی
        cpuUsageStr.erase(std::remove(cpuUsageStr.begin(), cpuUsageStr.end(), '\n'), cpuUsageStr.end());
        memUsageStr.erase(std::remove(memUsageStr.begin(), memUsageStr.end(), '\n'), memUsageStr.end());

        std::cout << "Raw CPU: [" << cpuUsageStr << "]" << std::endl;
        std::cout << "Raw MEM: [" << memUsageStr << "]" << std::endl;

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
                "ps -eo pid,%cpu --sort=-%cpu | awk 'NR==2 {print $1}'", port);
            pid.erase(std::remove(pid.begin(), pid.end(), '\n'), pid.end());
            std::cout << "Terminating process PID: " << pid << std::endl;
            runRemoteCommand(user, host, password, "kill -9 " + pid, port);
        }

        if (memUsage > memThreshold) {
            std::cout << "⚠️  Memory usage exceeds threshold!" << std::endl;
            std::string pid = runRemoteCommand(user, host, password,
                "ps -eo pid,%mem --sort=-%mem | awk 'NR==2 {print $1}'", port);
            pid.erase(std::remove(pid.begin(), pid.end(), '\n'), pid.end());
            std::cout << "Terminating process PID: " << pid << std::endl;
            runRemoteCommand(user, host, password, "kill -9 " + pid, port);
        }

        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}
