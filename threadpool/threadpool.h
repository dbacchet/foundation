#pragma once

#include "common/foundation_types.h"
#include "common/concurrent_queue.h"

#include <cstdint>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>

struct Job;

typedef void (*JobFunction)(Job*, void*);

/// \struct Job
/// \brief structure containing the job data (function and IO data)
/// \details this struct is defined to be exactly the same length of a cache line, to avoid
/// false-sharing problems.
struct Job {
    JobFunction function;
    Job* parent;
    // std::atomic<int32_t> unfinished_jobs; // atomic
    int32_t unfinished_jobs;
    char local_data[CACHE_LINE_SIZE-sizeof(function)-sizeof(Job*)-sizeof(unfinished_jobs)];
};


class ThreadPool {
public:
    enum State {
        PAUSED = 0, ///< thread pool is not executing jobs
        ACTIVE,     ///< thread pool active and processing/waiting for jobs
        STOPPED     ///< thread pool is stopped and will terminate
    };
public:
    ThreadPool(unsigned int num_worker_threads);
    ThreadPool(const ThreadPool&) = delete;
    virtual ~ThreadPool();

    /// add a job to the pool
    void add_job(const Job &job);

    size_t open_jobs()  {  return _jobs.size();  };

    /// start the workers
    void start();
    /// wait until all workers complete
    void wait();
    /// stop the execution
    void stop();
    /// clear the queue of open jobs;
    void clear();

// protected:

    ConcurrentQueue<Job>     _jobs;        /// job queue
    std::vector<std::thread> _workers;     /// worker thread pool
    std::mutex               _mutex;       /// sync mutex for the queue and the condition variable
    std::condition_variable  _cv;          /// condition variable used by the workers to wait and get notified
    State                    _state = PAUSED;
};

// Use a nice trick from this answer: http://stackoverflow.com/a/8438730/21475
// In order to get a numeric thread ID in a platform-independent way, we use a thread-local
// static variable's address as a thread identifier. Assume C++11 compliant compiler
const uint64_t invalid_thread_id  = 0;		// Address can't be nullptr
inline uint64_t thread_id() { static thread_local int x; return reinterpret_cast<uint64_t>(&x); }
