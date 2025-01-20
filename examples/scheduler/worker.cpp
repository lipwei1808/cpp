#include "Worker.hpp"

#include <iostream>

int main() {
    Worker worker{nullptr, "8999"};
    worker.connect();
    std::string x;
    std::cin >> x;
}
