#include "Master.hpp"
#include "Logger.hpp"

#include <csignal>

Master* master1;

void sighandler(int s) {
    master1->stop();
}

void masterThread() {
    Master master{nullptr, "8999"};
    master1 = &master;
    if (!master.init()) {
        LOG_ERROR("Error init master");
        return;
    }
    if (!master.listen()) {
        LOG_ERROR("Error listening on master");
        return;
    }
    master.run();
    LOG_INFO("Master thread ending!");
}

int main() {
    LOG_INFO("Starting master");
    std::thread t{masterThread};
    t.join();
}
