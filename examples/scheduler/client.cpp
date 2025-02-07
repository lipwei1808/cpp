#include "Logger.hpp"
#include "Network.hpp"
#include "String.hpp"
#include "Util.hpp"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

bool Send(int fd, const String& s) {
    if (send(fd, s.c_str(), s.size(), 0) == -1) {
        LOG_ERROR("Error sending data errno=%d, msg=%s", errno, strerror(errno));
        return false;
    }

    return true;
}

bool Receive(int fd, String& s) {
    char buffer[MAX_MESSAGE_SIZE];
    if (recv(fd, buffer, MAX_MESSAGE_SIZE, 0) == -1) {
        LOG_ERROR("Error sending data errno=%d, msg=%s", errno, strerror(errno));
        return false;
    }

    s = {buffer};
    return true;
}

int main() {
    int fd = connectToHost(nullptr, "8999");
    if (fd == -1) {
        LOG_ERROR("Error connecting to host");
        return 1;
    }

    while (true) {
        std::string line;
        getline(std::cin, line);
        std::cout << "Received: [" << line << "]\n";
        if (!Send(fd, {line.c_str()})) {
            continue;
        }
    }
    
    close(fd);
}
