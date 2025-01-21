#include "Worker.hpp"
#include "Logger.hpp"
#include "Vector.hpp"

#include <thread>

int main() {
    Vector<Worker> workers;
    for (int i = 0; i < 3; i++) {
        workers.emplace_back(nullptr, "8999");
    }

    LOG_TRACE("Middle");
    using namespace std::chrono_literals;
    for (int i = 0; i < workers.size(); i++) {
        std::this_thread::sleep_for(2s);
        workers[i].connect();
    }
}

