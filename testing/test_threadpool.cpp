#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

#include "gtest/gtest.h"

#include "threadpool/threadpool.h"

namespace {
    void my_job_func(Job *job) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // printf("job %d in thread %llu\n",(int)job->local_data[0],thread_id());
    }

    // support class for testing the job dependency
    struct PippoArray {
        PippoArray(unsigned int len)
        : ary(len,0), pos(0) {
        }

        void set_value(unsigned int pos, int val) {
            std::unique_lock<std::mutex> lock(_mutex);
            if (pos<ary.size())
                ary[pos] = val;
        }
        void ordered_fill(int val) {
            std::unique_lock<std::mutex> lock(_mutex);
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (pos<ary.size())
                ary[pos++] = val;
        }
        void print_data() const {
            for (auto &v: ary)
                printf("%d",v);
            printf("\n");
        }

        std::vector<int> ary;
        std::mutex _mutex;
        unsigned int pos;
    };

    struct PippoBridge {
        PippoArray *ary;
        int ciccio;
    };
    template <int val>
    void pippoarray_fun(Job *job) {
        // PippoArray *ary = (PippoArray*)job->local_data;
        PippoBridge &pb = *((PippoBridge*)job->local_data);
        auto ary = pb.ary;
        ary->ordered_fill(val);
    }
}

template <typename T>
void print_job_layout(const T &job) {
    auto len = sizeof(job);
    for (auto i=0; i<len; i++) {
        if (i%8==0)
            printf("\n");
        int8_t val = *((int8_t*)&job + i);
        printf("|%4d",val);
    }
    printf("\n");
}

TEST(BasicThreadPool, Job) {
    Job job;
    // print_job_layout(job);
    // a job must have the same length of a cache line
    ASSERT_EQ(sizeof(job),CACHE_LINE_SIZE);
    // a job should be zero initialized except for the parent id (that is the invalid id)
    ASSERT_EQ(job.function,nullptr);
    ASSERT_FALSE(valid(job.parent_id));
    ASSERT_EQ(job.unfinished_jobs,0);
    for (auto i=0; i<Job::local_data_len; i++)
        ASSERT_EQ(job.local_data[i],0);
}

TEST(BasicThreadPool, Creation) {
    BasicThreadPool pool(4);
    for (auto i=0; i<100; i++) {
        pool.add_job(Job(my_job_func,&i));
    }
    // start and wait until all the jobs are done
    pool.start();
    pool.wait();
    ASSERT_EQ(pool.open_jobs(),0);
}

TEST(BasicThreadPool, Stop) {
    BasicThreadPool pool(8);
    for (auto i=0; i<100; i++) {
        pool.add_job(Job(my_job_func,&i));
    }
    // start and stop before completion
    pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    pool.stop();
    pool.wait();
    ASSERT_GT(pool.open_jobs(),0);
}


TEST(ThreadPool, Creation) {
    ThreadPool pool(4);
    for (auto i=0; i<100; i++) {
        // Job job(my_job_func,&i);
        pool.add_job(Job(my_job_func,&i));
    }
    // start and wait until all the jobs are done
    pool.start();
    pool.wait();
    ASSERT_EQ(pool.open_jobs(),0);
}

TEST(ThreadPool, Stop) {
    ThreadPool pool(8);
    for (auto i=0; i<100; i++) {
        pool.add_job(Job(my_job_func,&i));
    }
    // start and stop before completion
    pool.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    pool.stop();
    pool.wait();
    ASSERT_GT(pool.open_jobs(),0);
}

TEST(ThreadPool, Depend) {
    ThreadPool pool(1);
    PippoArray pippoary(100);
    PippoBridge pb { .ary=&pippoary, .ciccio=1  };
    auto id1 = pool.add_locked_job(Job(pippoarray_fun<1>, pb));
    auto id2 = pool.add_locked_job(Job(pippoarray_fun<2>, pb, id1));
    for (auto i=2; i<100; i++) {
        pool.add_job(Job(pippoarray_fun<3>, pb, id2));
    }
    pool.unlock_job(id1);
    pool.unlock_job(id2);
    // start and wait until all the jobs are done
    pool.start();
    pool.wait();
    // pippoary.print_data();
    for (auto i=0; i<98; i++)
        ASSERT_EQ(pippoary.ary[i],3);
    ASSERT_EQ(pippoary.ary[98],2);
    ASSERT_EQ(pippoary.ary[99],1);
}

