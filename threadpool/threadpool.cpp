#include "threadpool.h"

// local variables and functions
namespace {
    // // Use a nice trick from this answer: http://stackoverflow.com/a/8438730/21475
    // // In order to get a numeric thread ID in a platform-independent way, we use a thread-local
    // // static variable's address as a thread identifier. Assume C++11 compliant compiler
    // const uint64_t invalid_thread_id  = 0;		// Address can't be nullptr
    // inline uint64_t thread_id() { static thread_local int x; return reinterpret_cast<uint64_t>(&x); }

    void worker_thread_func(ThreadPool *thread_pool) {
        // for(;;)
        while (thread_pool->_state!=ThreadPool::STOPPED)
        {
            Job job;
            {
                std::unique_lock<std::mutex> lock(thread_pool->_mutex);
                thread_pool->_cv.wait(lock, [thread_pool]{ return thread_pool->_state==ThreadPool::STOPPED
                                                               || (thread_pool->_state==ThreadPool::ACTIVE && thread_pool->_jobs.size()>0); });
                // if stopped, exit
                if (thread_pool->_state==ThreadPool::STOPPED) {
                    // printf("stop!!! (thread %llu)\n",thread_id());
                    return;
                }
                // here we are guaranteed that there is at least a job in the queue
                /// \todo remove the check or replace the message with proper logging
                if (!thread_pool->_jobs.pop_async(job)) {
                    printf("no more jobs in thread %llu\n",thread_id());
                    return;
                }
            }
            // printf("execute fun in thread %llu\n",thread_id());
            job.function(&job,nullptr);
        }
    }
}



ThreadPool::ThreadPool(unsigned int num_worker_threads) {
    // printf("size of Job struct: %lu\n", sizeof(Job));
    // printf("size of Job struct local_data: %lu\n", sizeof(Job::local_data));
    // printf("thread id: %llu\n", thread_id());
    // create the worker threads
    for (auto i=0; i<num_worker_threads; i++) {
        _workers.push_back(std::thread(worker_thread_func, this));
    }
}

ThreadPool::~ThreadPool() {
    stop();
    _cv.notify_all();
    for (auto&& w: _workers) {
        w.join();
    }
    _workers.clear();
}

// add a job to the pool
void ThreadPool::add_job(const Job &job) {
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _jobs.push(job);
    }
    _cv.notify_one();
}

// start the workers
void ThreadPool::start() {
    std::unique_lock<std::mutex> lock(_mutex);
    _state = ThreadPool::ACTIVE;
    _cv.notify_all();
}

// wait until all workers complete
void ThreadPool::wait() {
    while (_state!=ThreadPool::STOPPED && _jobs.size()>0)
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

// stop the execution
void ThreadPool::stop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _state = ThreadPool::STOPPED;
}

// clear the queue of open jobs;
void ThreadPool::clear() {

}

