#include "Master.hpp"
#include "Logger.hpp"
#include "Util.hpp"
#include "message.pb.h"
#include "UniquePtr.hpp"
#include "HeartbeatMonitor.hpp"
#include "Network.hpp"

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

using namespace std::chrono_literals;
Master::Master(const char* hostname, const char* port): fd(0),
        hostname(hostname), port(port),
        heartbeatMonitor{UniquePtr<HeartbeatMonitor>{new HeartbeatMonitor(this, 20s)}} {}

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
    
    // Start monitors
    heartbeatMonitor->activate();

    // Initialise kqueue
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
    struct timespec timeout;
    timeout.tv_sec = 5;
    timeout.tv_nsec = 0;

    LOG_INFO("Master running!");
    while (!shutdown) {
        LOG_TRACE("kevent loop start");
        int nfds = kevent(kq, nullptr, 0, eventList, N, &timeout);
        LOG_TRACE("kevent loop nfds=%d", nfds);
        if (nfds < 0) {
            LOG_ERROR("Error in kevent");
            return false;
        }

        for (int i = 0; i < nfds; i++) {
            if (eventList[i].flags & EV_EOF) {
                handleDisconnect(eventList[i].ident);
                continue;
            }

            int rfd = eventList[i].ident;
            if (rfd == fd) {
                handleNewConnection();
                continue;
            } 

            String s;
            if (!Receive(rfd, s)) {
                LOG_ERROR("Error receiving data from fd=%d", rfd);
                handleDisconnect(rfd);
                continue;
            }

            if (!handle(rfd, s)) {
                LOG_ERROR("Error handling data fd=%d", rfd);
                handleDisconnect(rfd);
            }
        }
    }
    LOG_INFO("Master run loop ended!");
    barrier.arrive_and_wait();
    return true;
}

bool Master::handle(int fd, const String& s) {
    Scheduler::Message message;
    if (!message.ParseFromString(s.toStdString())) {
        LOG_ERROR("Error deserializing mesage from fd=%d", fd);
        return false;
    }

    bool res = true;
    switch (message.type()) {
        case (Scheduler::MessageType::MESSAGE_TYPE_HANDSHAKE_REQ): {
            res = sendHandshakeResponse(fd);
            break;
        }
        default: {
            if (workerFds.contains(fd)) {
                res = handleWorker(fd, message);
            } else if (clientFds.contains(fd)) {
                res = handleClient(fd, message);
            } else {
                res = false;
            }
        }
    }
    return res;
}
 
bool Master::handleClient(int clientFd, const Scheduler::Message& msg) {
    return true;
}

bool Master::handleWorker(int workerFd, const Scheduler::Message& message) {
    bool res = true;
    switch (message.type()) {
        case (Scheduler::MessageType::MESSAGE_TYPE_HEARTBEAT): {
            res = handleHeartbeat(workerFd);
            break;
        }
        case (Scheduler::MessageType::MESSAGE_TYPE_HANDSHAKE_REQ): {
            res = sendHandshakeResponse(workerFd);
            break;
        }
        case (Scheduler::MessageType::MESSAGE_TYPE_TASK_RES): {
            res = handleTaskResponse(workerFd, message.data());
            break;
        }
        default: {
            break;
        }
    }

    return res;
}

void Master::handleDisconnect(int fd) {
    if (workerFds.contains(fd)) {
        handleDisconnectWorker(fd);
    } else {
        LOG_ERROR("Attempting to disconnect an unknown worker fd=%d", fd);
        return;
    }

    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
    if (kevent(kq, &kev, 1, nullptr, 0, nullptr) == -1) {
        LOG_ERROR("Error removing socket fd=%d from event list %d: %s", fd, errno, strerror(errno));
    }
    close(fd);
}

void Master::handleDisconnectWorker(int workerFd) {
    LOG_INFO("Disconnect workerFd=%d", fd);
    heartbeatMonitor->disconnectWorker(workerFds.at(workerFd));
    workerFds.erase(workerFd);
}

bool Master::sendHandshakeResponse(int workerFd) {
    LOG_TRACE("Sending handshake response to workerfd=%d", workerFd);
    if (workerFds.contains(workerFd)) {
        LOG_ERROR("Handshake requested from a worker that has already shook hands! fd=%d", workerFd);
        return false;
    }

    int id = workerFd;
    Scheduler::HeartbeatData hData;
    hData.set_id(id);
    std::string serialized;
    if (!hData.SerializeToString(&serialized)) {
        LOG_ERROR("Error serializing heartbeat data workerFd=%d, workerId=%d", workerFd, workerFds.at(workerFd));
        return false;
    }

    Scheduler::Message msg;
    msg.set_type(Scheduler::MessageType::MESSAGE_TYPE_HANDSHAKE_RES);
    msg.set_data(serialized);

    if (!msg.SerializeToString(&serialized)) {
        LOG_ERROR("Error serializing msg heartbeat data workerFd=%d, workerId=%d", workerFd, id);
        return false;
    }

    if (send(workerFd, serialized.data(), serialized.size(), 0) == -1) {
        LOG_ERROR("Error sending heartbeat %d: %s", errno, strerror(errno));
        return false;
    }
    distributor.addWorker(workerFd);
    workerFds.insert({workerFd, id});
    heartbeatMonitor->addWorker(id);
    return true;
}

bool Master::handleHeartbeat(int workerFd) {
    if (fd == 0) {
        return false;
    }

    if (!workerFds.contains(workerFd)) {
        LOG_ERROR("WorkerFD=%d unknown", workerFd);
        return false;
    }

    // TODO: Keep track of heartbeats and disconnect if not recv
    heartbeatMonitor->registerHeartbeat(workerFds.at(workerFd));
    return true;
}

void Master::handleNewConnection() {
    LOG_TRACE("Accepting new connection");
    sockaddr addr;
    socklen_t socklen = sizeof(addr);
    int newFd;
    if ((newFd = accept(fd, &addr, &socklen)) == -1) {
        LOG_ERROR("Error accepting new connection %d", errno);
        return;
    }
    struct kevent evSet;
    EV_SET(&evSet, newFd, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq, &evSet, 1, nullptr, 0, nullptr) == -1) {
        LOG_ERROR("Error in kevent when adding new connection %d", errno);
        close(newFd);
        return;
    }
    LOG_INFO("Accepted new connection fd=%d", newFd);
}

bool Master::handleTaskResponse(int workerFd, const std::string& data) {
    Scheduler::TaskResponse msg;
    if (!msg.ParseFromString(data)) {
        LOG_ERROR("Error deserializing task response from worker=%d", workerFd);
        return false;
    }
    distributor.addWorker(workerFd);
    return true;
}

void Master::stop() {
    shutdown = true;
    barrier.arrive_and_wait();
}

Master::~Master() {
    LOG_TRACE("Master destructor");
    close(fd);
    heartbeatMonitor->stop();
}
