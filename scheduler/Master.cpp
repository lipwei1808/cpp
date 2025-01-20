#include "Master.hpp"
#include "Logger.hpp"
#include "Util.hpp"
#include "message.pb.h"

#include <arpa/inet.h>
#include <cstring>
#include <cerrno>
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

Master::Master(const char* hostname, const char* port): fd(0), hostname(hostname), port(port) {}

bool Master::init() {
    addrinfo *res, hints, *p;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0) {
        LOG_TRACE("Error: %s", gai_strerror(status));
    }

    for (p = res; res != nullptr; res = res->ai_next) {
        auto [ipver, addr] = getInAddr(p);
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        LOG_TRACE("%s: %s", ipver.c_str(), ipstr);

        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            LOG_ERROR("Error creating socket %d: %s", errno, strerror(errno));
            continue;
        }

        int yes = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            LOG_ERROR("Error setsocketopt: %d: %s", errno, strerror(errno));
            continue;
        }

        if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(fd);
            LOG_ERROR("Error bind: %d", errno);
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if (p == nullptr) {
        LOG_ERROR("Error failed to create socket");
        return false;
    }

    return true;
}

bool Master::listen() {
    if (fd == 0) {
        return false;
    }

    if (::listen(fd, 10) == -1) {
        LOG_ERROR("Error failed to create socket");
        return false;
    }

    LOG_INFO("Master listening on port %s!", port);
    return true;
}

bool Master::run() {
    if (fd == 0) {
        return false;
    }

    // Initialise kqueue
    int kq;
    struct kevent evSet;
    if ((kq = kqueue()) == -1) {
        LOG_ERROR("Error initialising kqueue %d", errno);
        return false;
    }

    // Listen on master socket
    EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq, &evSet, 1, nullptr, 0, nullptr) == -1) {
        LOG_ERROR("Error kevent when reading from main connection socket");
        return false;
    }

    int N = 10;
    struct kevent eventList[N];

    bool keepAlive = true;
    LOG_INFO("Master running!");
    while (keepAlive) {
        int nfds = kevent(kq, nullptr, 0, eventList, N, nullptr);
        LOG_TRACE("kevent loop nfds=%d", nfds);
        if (nfds < 1) {
            LOG_ERROR("Error in kevent");
            return false;
        }

        for (int i = 0; i < nfds; i++) {
            if (eventList[i].ident == fd) {
                LOG_TRACE("Accepting new connection");
                sockaddr addr;
                socklen_t socklen = sizeof(addr);
                int newFd;
                if ((newFd = accept(fd, &addr, &socklen)) == -1) {
                    LOG_ERROR("Error accepting new connection %d", errno);
                    continue;
                }
                LOG_INFO("Accepted new connection fd=%d", newFd);
                EV_SET(&evSet, newFd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
                if (kevent(kq, &evSet, 1, nullptr, 0, nullptr) == -1) {
                    LOG_ERROR("Error in kevent when adding new connection %d", errno);
                }
            } else {
                handleWorker(eventList[i].ident);
            }
        }
    }
    return true;
}

void Master::handleWorker(int workerFd) {
    char buffer[MAX_MESSAGE_SIZE];
    ssize_t bytes = recv(workerFd, buffer, MAX_MESSAGE_SIZE, 0);
    if (bytes <= 0) {
        LOG_ERROR("Error reveiving message. bytes=%zu", bytes);
        return;
    }

    Scheduler::Message message;
    if (!message.ParseFromString(buffer)) {
        LOG_ERROR("Error deserializing mesage from worker. fd=%d", workerFd);
        return;
    }

    LOG_TRACE("Master successfully received message from worker");
    if (message.type() == Scheduler::MessageType::MESSAGE_TYPE_HEARTBEAT) {
        sendHeartbeat(workerFd);
    }

    return;

}

bool Master::sendHeartbeat(int workerFd) {
    if (fd == 0) {
        return false;
    }

    LOG_INFO("Master sending heartbeat");
    Scheduler::Message msg;
    msg.set_type(Scheduler::MessageType::MESSAGE_TYPE_HEARTBEAT);
    std::string res;
    if (!msg.SerializeToString(&res)) {
        LOG_ERROR("Error serializing heartbeat message");
        return false;
    }
    LOG_TRACE("Sending heartbeat now");
    if (send(workerFd, res.data(), res.size(), 0) == -1) {
        LOG_ERROR("Error sending heartbeat %d: %s", errno, strerror(errno));
        return false;
    }

    return true;
}

Master::~Master() {
    close(fd);
}
