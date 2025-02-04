#include "Worker.hpp"
#include "Logger.hpp"
#include "Vector.hpp"

#include <thread>

void runWorker(int s) {
    Worker worker{nullptr, "8999", std::chrono::seconds{s}};
    if (!worker.connect()) {
        return;
    }
    worker.run();
}

int main(int argc, char** argv) {
    int cnt = 1;
    int s = 1;
    if (argc > 1) {
        char* arg = argv[1];
        cnt = atoi(arg);
    }
    if (argc > 2) {
        char* arg = argv[2];
        s = atoi(arg);
    }
    LOG_INFO("Worker count=%d", cnt);
    Vector<std::thread> workers;
    using namespace std::chrono_literals;
    for (int i = 0; i < cnt; i++) {
        std::this_thread::sleep_for(2s);
        workers.emplace_back(std::move(std::thread{runWorker, s}));
    }
    for (auto& t: workers) {
        t.join();
    }
}

