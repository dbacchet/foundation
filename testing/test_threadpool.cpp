#include <iostream>
#include <thread>

#include "gtest/gtest.h"

#include "threadpool/threadpool.h"

namespace {
    void my_job_func(Job *job, void *data) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // printf("job %d in thread %llu\n",(int)job->local_data[0],thread_id());
    }
}

TEST(ThreadPool, Creation) {
    ThreadPool pool(4);
    for (auto i=0; i<100; i++) {
        Job job;
        job.function = my_job_func;
        job.local_data[0] = i;
        pool.add_job(job);
    }
    // start and wait until all the jobs are done
    pool.start();
    pool.wait();
    ASSERT_EQ(pool.open_jobs(),0);
}

TEST(ThreadPool, Stop) {
    ThreadPool pool(8);
    for (auto i=0; i<100; i++) {
        Job job;
        job.function = my_job_func;
        job.local_data[0] = i;
        pool.add_job(job);
    }
    // start and stop before completion
    pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    pool.stop();
    pool.wait();
    ASSERT_GT(pool.open_jobs(),0);
}


