#include <gtest/gtest.h>
#include <thread>

#include "SharedPtr.hpp"
#include "Logger.hpp"

void copy_into_thread(int k, SharedPtr<int> ptr)
{
    if(k == 0)
        return;

    std::thread([](int k, SharedPtr<int> ptr) {
        LOG_TRACE("%d: ptr_val: %d", k, *ptr);
        copy_into_thread(k, ptr);
    }, k - 1, ptr).detach();
}

TEST(SharedPtrTest, Constructor) {

    SharedPtr<int> x = SharedPtr(new int(888));
    copy_into_thread(10, x);
}
