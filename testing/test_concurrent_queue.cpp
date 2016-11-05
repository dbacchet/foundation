#include <iostream>
#include <thread>

#include "gtest/gtest.h"

#include "common/concurrent_queue.h"

namespace {
    void add_elements(ConcurrentQueue<int> *q, int base) {
        for (auto i=0; i<100; i++)
            q->push(base+i);
    }
    int get_one(ConcurrentQueue<int> *q) {
        return q->pop();
    }
    void pop_elements(ConcurrentQueue<int> *q, int num) {
        for (auto i=0; i<num; i++)
            q->pop();
    }
}

TEST(ConcurrentQueue, Creation) {
    ConcurrentQueue<int> q;
    ASSERT_EQ(q.size(),0);
    q.push(1);
    ASSERT_EQ(q.size(),1);
}

TEST(ConcurrentQueue, push) {
    ConcurrentQueue<int> q;
    std::thread t1(add_elements, &q, 0);
    std::thread t2(add_elements, &q, 100);
    std::thread t3(add_elements, &q, 200);
    std::thread t4(add_elements, &q, 300);
    std::thread t5(add_elements, &q, 400);
    std::thread t6(pop_elements, &q, 50);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    ASSERT_EQ(q.size(),450);
}

