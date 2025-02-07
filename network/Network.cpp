#include "Logger.hpp"
#include "Util.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int connectToHost(const char* hostname, const char* port) {
    struct addrinfo *p, *res, hints;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        LOG_ERROR("Unable to get address info for hostname=%s, port=%s, error=%s", hostname, port, gai_strerror(status));
        return -1;
    }

    int fd;
    for (p = res; p != nullptr; p = p->ai_next) {
        auto [ipver, addr] = getInAddr(p);
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        LOG_INFO("IP Type: [%s], Str: [%s]", ipver.c_str(), ipstr);
        
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            LOG_ERROR("Error creating socket %d: %s", errno, strerror(errno));
            continue;
        }

        if (::connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
            LOG_ERROR("Error connecting: %d, %s", errno, strerror(errno));
            continue;
        }
        break;
    }

    freeaddrinfo(res);
    if (p == nullptr) {
        LOG_ERROR("Could not find a host to connect to");
        return -1;
    }

    return fd;
}

