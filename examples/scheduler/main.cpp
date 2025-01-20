#include "Master.hpp"
#include "Worker.hpp"
#include "Logger.hpp"


void masterThread() {
    Master master{nullptr, "8999"};
    if (!master.init()) {
        LOG_ERROR("Error init master");
        return;
    }
    if (!master.listen()) {
        LOG_ERROR("Error listening on master");
        return;
    }
    master.run();
}

void workerThread() {
    Worker worker{"", "8999"};
    if (!worker.connect()) {
        LOG_ERROR("Worker connect failed, ending now.");
    }
}

int main() {
    using namespace std::chrono_literals;
    LOG_INFO("Starting master");
    masterThread();
}
