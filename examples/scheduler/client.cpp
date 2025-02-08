#include "Logger.hpp"
#include "Network.hpp"
#include "Util.hpp"

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

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
