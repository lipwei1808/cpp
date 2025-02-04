#include <iostream>

int main() {
    while (true) {
        std::string line;
        getline(std::cin, line);
        std::cout << "Received: [" << line << "]\n";
    }
}
