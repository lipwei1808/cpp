#include "Distributor.hpp"
#include "message.pb.h"
#include "Util.hpp"

#include <thread>

void Distributor::addTask(const Scheduler::Task& task) {
    taskQueue.push(std::move(task));
}

void Distributor::addWorker(int workerFd) {
    workerQueue.push(workerFd);
}

void Distributor::start() {
    svcThread = std::thread{&Distributor::svc, this};
}

void Distributor::svc() {
    while (!shutdown) {
        int workerFd;
        bool success = workerQueue.consumeSync(workerFd);
        if (!success || shutdown) {
            break;
        }

        Scheduler::Task task;
        success = taskQueue.consumeSync(task);
        if (!success || shutdown) {
            break;
        }

        if (!sendTask(workerFd, task)) {
            LOG_ERROR("Error in sending task, ending svc loop");
            break;
        }
    }
}

bool Distributor::sendTask(int workerFd, const Scheduler::Task& task) {
    Scheduler::Message msg;
    msg.set_type(Scheduler::MessageType::MESSAGE_TYPE_TASK_REQ);

    std::string taskSerialized;
    if (!task.SerializeToString(&taskSerialized)) {
        LOG_ERROR("Error serializing task msg");
        return false;
    }

    msg.set_data(taskSerialized);

    std::string buffer;
    if (!msg.SerializeToString(&buffer)) {
        LOG_ERROR("Error serializing data to string. workerFd=%d", workerFd);
        return false;
    }

    if (send(workerFd, buffer.data(), buffer.size(), 0) == -1) {
        LOG_ERROR("Error sending data to worker=%d", workerFd);
        return false;
    }

    return true;
}

void Distributor::stop() {
    shutdown = true;
    taskQueue.stop();
    workerQueue.stop();
    if (svcThread.joinable()) {
        svcThread.join();
    }
}

