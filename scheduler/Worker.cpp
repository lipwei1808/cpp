#include "Worker.hpp"

#include "Logger.hpp"
#include "message.pb.h"
#include "Util.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <netdb.h>
#include <unistd.h>

Worker::Worker(const char* hostname, const char* port): fd(0), hostname(hostname), port(port) {}

Worker::Worker(Worker&& worker): fd(worker.fd), hostname(worker.hostname), port(worker.port) {
    LOG_TRACE("Worker constructed");
    worker.fd = 0;
}

bool Worker::connect() {
    addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int status;
    if ((status = getaddrinfo(hostname, port, &hints, &res)) != 0) {
        LOG_ERROR("Error: %s", gai_strerror(status));
        return false;
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        auto [ipver, addr] = getInAddr(p);
        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        LOG_TRACE("%s: %s", ipver.c_str(), ipstr);
        if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            LOG_ERROR("Error creating socket %d: %s", errno, strerror(errno));
            continue;
        }

        if (::connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
            LOG_ERROR("Error connecting to master %d: %s", errno, strerror(errno));
            close(fd);
            continue;
        } 
        break;
    }
    if (p == nullptr) {
        LOG_ERROR("Could not find a host to connect");
        return false;
    }
    LOG_INFO("Worker connected");
    freeaddrinfo(res);

    heartbeatThread = std::thread{&Worker::runHeartbeat, this};
    return true;
}

void Worker::runHeartbeat() {
    using namespace std::chrono_literals;
    int failedCount = 0;
    LOG_INFO("Initialising heartbeat thread");
    while (true) {
        std::this_thread::sleep_for(5s);
        if (!sendHeartbeat()) {
            failedCount++;
            if (failedCount > maxHeartbeatRetries) {
                LOG_ERROR("Failed heartbeat for %d times, exiting worker", maxHeartbeatRetries);
                break;
            }
            continue;
        }
        failedCount = 0;
    }
}

bool Worker::sendHeartbeat() {
    if (fd == 0) {
        return false;
    }

    LOG_INFO("Worker sending heartbeat");
    Scheduler::Message msg;
    msg.set_type(Scheduler::MessageType::MESSAGE_TYPE_HEARTBEAT);
    std::string res;
    if (!msg.SerializeToString(&res)) {
        LOG_ERROR("Error serializing heartbeat message");
        return false;
    }
    LOG_TRACE("Sending heartbeat now");
    if (send(fd, res.data(), res.size(), 0) == -1) {
        LOG_ERROR("Error sending heartbeat %d: %s", errno, strerror(errno));
        return false;
    }

    char buffer[MAX_MESSAGE_SIZE];
    LOG_TRACE("Receiving heartbeat response from master now");
    ssize_t bytes = recv(fd, buffer, MAX_MESSAGE_SIZE, 0);
    if (bytes <= 0) {
        LOG_ERROR("Error receiving heartbeat. bytes=%zu. %d: %s", bytes, errno, strerror(errno));
        return false;
    }
    Scheduler::Message response;
    if (!response.ParseFromString(buffer)) {
        LOG_ERROR("Error deserializing heartbeat response from master");
        return false;
    }
    

    bool ok = response.type() == Scheduler::MessageType::MESSAGE_TYPE_HEARTBEAT;
    LOG_TRACE("Heartbeat cycle is %s", ok ? "ok" : "not ok");
    return ok;
}

Worker::~Worker() {
    close(fd);
}

