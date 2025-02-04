#include "Worker.hpp"

#include "Logger.hpp"
#include "message.pb.h"
#include "Util.hpp"
#include "Network.hpp"

#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <netdb.h>
#include <unistd.h>

Worker::Worker(const char *hostname, const char *port,
        std::chrono::seconds heartbeatInterval) : fd(0), hostname(hostname),
        port(port), id(-1), heartbeatInterval(heartbeatInterval) {}

Worker::Worker(Worker &&worker): fd(worker.fd), hostname(worker.hostname),
    port(worker.port), heartbeatThread(std::move(worker.heartbeatThread)),
    id(worker.id), heartbeatInterval(worker.heartbeatInterval)
{
    LOG_TRACE("Worker move constructed");
}

bool Worker::connect()
{
    int rfd = connectToHost(hostname, port);
    if (rfd == -1)
    {
        LOG_ERROR("Failed to connect");
        return false;
    }
    fd = rfd;
    LOG_INFO("Worker connected");

    if (!handshake())
    {
        LOG_ERROR("Handshake with master failed");
        close(fd);
        return false;
    }
    return true;
}

void Worker::run()
{
    if (id == -1) {
        LOG_ERROR("Trying to run without id initialised");
        return;
    }
    heartbeatThread = std::thread{&Worker::runHeartbeat, this};

    while (true)
    {
        char buffer[MAX_MESSAGE_SIZE];
        ssize_t bytes = recv(fd, buffer, MAX_MESSAGE_SIZE, 0);
        if (bytes == 0)
        {
            LOG_INFO("Master disconnected");
            break;
        }
        if (bytes == -1)
        {
            LOG_ERROR("Worker %d error receiving data. errno=%d, strerrror=%s", id, errno, strerror(errno));
            break;
        }
        Scheduler::Message msg;
        if (!msg.ParseFromString(std::string{buffer, static_cast<size_t>(bytes)}))
        {
            LOG_ERROR("Worker %d error deserializing message", id);
            continue;
        }

        if (msg.type() != Scheduler::MessageType::MESSAGE_TYPE_TASK_REQ)
        {
            LOG_ERROR("Worker %d invalid message type here type=%d", id, msg.type());
            continue;
        }

        Scheduler::Task task;
        if (!task.ParseFromString(msg.data()))
        {
            LOG_ERROR("Worker %d error parsing data task from message type=%d", id, msg.type());
            continue;
        }

        bool res = execute(task.type());

        Scheduler::TaskResponse taskResponse;
        taskResponse.set_success(res);
        std::string r;
        if (!taskResponse.SerializeToString(&r))
        {
            LOG_ERROR("Worker %d error serializing taskResponse success=%d", id, res);
            continue;
        }
        Scheduler::Message response;
        response.set_type(Scheduler::MessageType::MESSAGE_TYPE_TASK_RES);
        response.set_data(r);
        if (!response.SerializeToString(&r))
        {
            LOG_ERROR("Worker %d error serializing response success=%d", id, res);
            continue;
        }

        if (send(fd, r.data(), r.size(), 0) == -1)
        {
            LOG_ERROR("Worker %d error sending response to master errno=%d, %s", id, errno, strerror(errno));
            break;
        }
    }

    stopHeartbeat();
}

void Worker::stopHeartbeat() {
    LOG_INFO("Worker %d stopping heartbeat thread", id);
    shutdownHeartbeat = true;
    if (heartbeatThread.joinable()) {
        heartbeatThread.join();
    }
    shutdownHeartbeat = false;
}

bool Worker::execute(Scheduler::TaskType taskType)
{
    switch (taskType)
    {
    case (Scheduler::TaskType::TASK_ONE):
    {
        executeTaskOne();
        break;
    }
    case (Scheduler::TaskType::TASK_TWO):
    {
        executeTaskTwo();
        break;
    }
    default:
    {
        return false;
    }
    }
    return true;
}

bool Worker::executeTaskOne()
{
    using namespace std::chrono_literals;
    LOG_INFO("Executing task one");
    std::this_thread::sleep_for(2s);
    LOG_INFO("Completed task one execution");
    return true;
}

bool Worker::executeTaskTwo()
{
    using namespace std::chrono_literals;
    LOG_INFO("Executing task two");
    std::this_thread::sleep_for(10s);
    LOG_INFO("Completed task two execution");
    return true;
}

bool Worker::handshake()
{
    Scheduler::Message msg{};
    msg.set_type(Scheduler::MessageType::MESSAGE_TYPE_HANDSHAKE_REQ);
    std::string res;
    if (!msg.SerializeToString(&res))
    {
        LOG_ERROR("Error serializing handshake message");
        return false;
    }

    LOG_TRACE("Sending handshake now(%zu): %s", res.size(), res.c_str());
    if (send(fd, res.data(), res.size(), 0) == -1)
    {
        LOG_ERROR("Error sending handshake %d: %s", errno, strerror(errno));
        return false;
    }

    char buffer[MAX_MESSAGE_SIZE];
    LOG_TRACE("Receiving handshake response from master now");
    ssize_t bytes = recv(fd, buffer, MAX_MESSAGE_SIZE, 0);
    if (bytes == 0)
    {
        LOG_INFO("Master disconnected!");
        return false;
    }
    if (bytes < 0)
    {
        LOG_ERROR("Error receiving handshake. bytes=%zu. %d: %s", bytes, errno, strerror(errno));
        return false;
    }

    Scheduler::Message response;
    if (!response.ParseFromString(std::string{buffer, static_cast<size_t>(bytes)}))
    {
        LOG_ERROR("Error parsing response from master");
        return false;
    }

    if (response.type() != Scheduler::MessageType::MESSAGE_TYPE_HANDSHAKE_RES)
    {
        LOG_ERROR("Invalid response from master for handshake");
        return false;
    }

    Scheduler::HeartbeatData data;
    if (!data.ParseFromString(response.data()))
    {
        LOG_ERROR("Error parsing heartbeat resopnse data from master");
        return false;
    }

    id = data.id();
    LOG_INFO("Handshake success, assigned id=%d", id);
    return true;
}

void Worker::runHeartbeat()
{
    LOG_INFO("Initialising heartbeat thread worker=%d", id);
    while (!shutdownHeartbeat)
    {
        std::this_thread::sleep_for(heartbeatInterval);
        if (shutdownHeartbeat || !sendHeartbeat())
        {
            break;
        }
    }
    LOG_TRACE("Run heartbeat thread ended, worker=%d", id);
}

bool Worker::sendHeartbeat()
{
    if (fd == 0)
    {
        return false;
    }

    LOG_INFO("Worker %d sending heartbeat", id);
    Scheduler::Message msg;
    msg.set_type(Scheduler::MessageType::MESSAGE_TYPE_HEARTBEAT);
    std::string res;
    if (!msg.SerializeToString(&res))
    {
        LOG_ERROR("Error serializing heartbeat message worker=%d", id);
        return false;
    }
    LOG_TRACE("Worker %d sending heartbeat now", id);
    if (send(fd, res.data(), res.size(), 0) == -1)
    {
        LOG_ERROR("Error sending heartbeat worker=%d errno=%d: %s", id, errno, strerror(errno));
        return false;
    }
    return true;
}

Worker::~Worker()
{
    stopHeartbeat();
    close(fd);
    LOG_TRACE("Worker %d destructor completed", id);
}
