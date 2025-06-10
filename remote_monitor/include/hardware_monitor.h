
#ifndef HARDWARE_MONITOR_H
#define HARDWARE_MONITOR_H

#include <string>

void monitorSystem(const std::string& user, const std::string& host, const std::string& password, float cpuThreshold, float memThreshold, int port);

#endif
