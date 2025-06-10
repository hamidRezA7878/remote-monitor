
#ifndef SSH_CONNECTION_H
#define SSH_CONNECTION_H

#include <string>

bool establishSSHConnection(const std::string& user, const std::string& host, const std::string& password, int port);
std::string runRemoteCommand(const std::string& user, const std::string& host, const std::string& password, const std::string& command, int port);

#endif
