#pragma once

#include "message.pb.h"
#include "TsQueue.hpp"

#include <thread>

class Distributor {
public:
    void addTask(const Scheduler::Task& task);
    void addWorker(int workerFd);
    void svc();
    void start();
    void stop();

private:
    bool sendTask(int workerFd, const Scheduler::Task& task);
    TsQueue<Scheduler::Task> taskQueue;
    TsQueue<int> workerQueue;
    bool shutdown = false;
    std::thread svcThread;
};
