#include "mpmc_queue.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>

std::mutex m;

void sync_print(const std::string &s, int64_t val) {
    std::lock_guard<std::mutex> lock(m);
    std::cout << s << val << std::endl;
}

int main(int argc, char *argv[]) {
    constexpr size_t cache_line = 128;
    containers::MPMCQueue<int64_t, cache_line> q(10);

    auto t1a = std::thread([&]() {
        int64_t idx = 100;
        for (int i = 0; i < 10; i++) {
            idx++;
            q.push(idx);
            sync_print("pushed ", idx);
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    auto t1b = std::thread([&]() {
        int64_t idx = 200;
        for (int i = 0; i < 10; i++) {
            idx++;
            q.push(idx);
            sync_print("pushed ", idx);
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    auto t1c = std::thread([&]() {
        int64_t idx = 300;
        for (int i = 0; i < 10; i++) {
            idx++;
            q.push(idx);
            sync_print("pushed ", idx);
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
    auto t2a = std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int64_t idx = -1;
        /* while (q.try_pop(idx)) { */
        while (!q.empty()) {
            q.pop(idx);
            sync_print("            popped (t2a)", idx);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        sync_print("queue empty", 0);
    });
    auto t2b = std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        int64_t idx = -1;
        while (q.try_pop(idx)) {
            q.pop(idx);
            sync_print("            popped (t2b)", idx);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        sync_print("queue empty", 0);
    });

    t1a.join();
    t1b.join();
    t1c.join();
    t2a.join();
    t2b.join();

    return 0;
}
