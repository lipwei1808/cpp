#include <gtest/gtest.h>

#include "Master.hpp"
#include "Vector.hpp"
#include "Worker.hpp"

#include <vector>

TEST(MasterTest, Constructor) {
    Master master{nullptr, "3000"};
    //Worker worker{nullptr, "3000"};
    Vector<Worker> workers;
    for (int i = 0; i < 3; i++) {
        //workers.emplace_back(nullptr, "3000");
    }
}
