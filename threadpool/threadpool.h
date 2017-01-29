#pragma once

#include "common/foundation_types.h"
#include "common/idtable.h"

#include <cstdint>
#include <cstring>
#include <atomic>
// #include <list>
#include <set>
#include <deque>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>

struct Job;

typedef void (*JobFunction)(Job*);


/// \struct Job
/// \brief structure containing the job data (function and IO data)
/// \details this struct is defined to be exactly the same length of a cache line, to avoid
/// false-sharing problems. On creation it is zero initialized, except for the parent_id that is set to the invalid ID.
/// \todo find a more elegant set of constuctors
struct Job {
    JobFunction function;
    // Job* parent;
    ID parent_id;
    // std::atomic<int32_t> unfinished_jobs; // atomic
    int32_t unfinished_jobs;
    static const unsigned local_data_len = CACHE_LINE_SIZE-sizeof(function)-sizeof(parent_id)-sizeof(unfinished_jobs);
    char local_data[local_data_len];

    // constructors
    Job()
    : function(nullptr), parent_id(ID()), unfinished_jobs(0) {
        memset(local_data,0,sizeof(local_data));
    }
    Job(const JobFunction &func)
    : function(func), parent_id(ID()), unfinished_jobs(0){
        memset(local_data,0,sizeof(local_data));
    }
    template <typename T>
    Job(const JobFunction &func, const T &func_data, ID parent_job=ID(), int32_t num_unfinished_jobs=0)
    : function(func), parent_id(parent_job), unfinished_jobs(num_unfinished_jobs){
        memset(local_data,0,sizeof(local_data));
        memcpy(local_data,&func_data,sizeof(T));
    }
};

    // template <>
    // inline Job::Job<void*>(const JobFunction &func, const void *ptr, ID parent_job, int32_t num_unfinished_jobs)
    // : function(func), parent_id(parent_job), unfinished_jobs(num_unfinished_jobs){
    //     memset(local_data,0,sizeof(local_data));
    //     // memcpy(local_data,func_data,sizeof(T));
    // }

/// \enum ThreadPoolState
enum class ThreadPoolState {
    PAUSED = 0, ///< thread pool is not executing jobs
    ACTIVE,     ///< thread pool active and processing/waiting for jobs
    STOPPED     ///< thread pool is stopped and will terminate
};


/// \class BasicThreadPool
/// \brief simple thred pool, with no job dependencies
/// \details the jobs are scheduled in the order they are submitted to the queue, without checking for dependencies.
class BasicThreadPool {
public:
    BasicThreadPool(unsigned int num_worker_threads);
    BasicThreadPool(const BasicThreadPool&) = delete;
    virtual ~BasicThreadPool();

    /// add a job to the pool
    /// \return the id of the job in the pool
    ID add_job(const Job &job);

    size_t open_jobs()  {  return _open_jobs_queue.size();  };

    /// get the next open job.
    /// \return true if the job has been extracted, false otherwise
    bool get_next_job(Job &job);

    /// start the workers
    void start();
    /// wait until all workers complete
    void wait();
    /// stop the execution
    void stop();
    /// clear the queues
    void clear();

protected:

    std::deque<ID>           _open_jobs_queue; /// queue of the jobs that are ready to be executed
    IDTable<Job>             _jobs;            /// table with all the jobs
    std::vector<std::thread> _workers;         /// worker thread pool
    std::mutex               _mutex;           /// sync mutex for the queue and the condition variable
    std::condition_variable  _cv;              /// condition variable used by the workers to wait and get notified
    ThreadPoolState          _state = ThreadPoolState::PAUSED;

    /// worker thread
    void worker_thread_function(void);
    /// getthe next open job. This function is not thread safe.
    bool get_next_job_unsafe(Job &job);
};


/// \class ThreadPool
/// \brief thread pool, with job dependencies
/// \details jobs are scheduled only when all the dependent jobs are done.
class ThreadPool: public BasicThreadPool {
public:
    ThreadPool(unsigned int num_worker_threads);
    ThreadPool(const ThreadPool&) = delete;
    virtual ~ThreadPool();

    /// add a job to the locked jobs queue. The number of unfinished jobs is set to 1, to avoid immediate execution.
    /// \return ID of the created job
    ID add_locked_job(const Job &job);
    /// close an open job. when an open job is closed, can be scheduled for execution.
    /// This function will decrease the number of unfinished jobs of 1.
    void unlock_job(ID job_id);

    /// wait unti all workers complete
    void wait();
    /// clear the queues
    void clear();

protected:

    std::set<ID> _locked_jobs; /// queue of the jobs that depend on unfinished jobs

    /// worker thread
    void worker_thread_function(void);
};

//
// Use a nice trick from this answer: http://stackoverflow.com/a/8438730/21475
// In order to get a numeric thread ID in a platform-independent way, we use a thread-local
// static variable's address as a thread identifier. Assume C++11 compliant compiler
const uint64_t invalid_thread_id  = 0;		// Address can't be nullptr
inline uint64_t thread_id() { static thread_local int x; return reinterpret_cast<uint64_t>(&x); }



// template functions implementation


// template <typename T>
// ID BasicThreadPool::add_job(const JobFunction &job_func, const T *func_data) {
//     auto job_id = _jobs.add(Job(job_func,func_data));
//     _open_jobs_queue.push_back(job_id);
//     return job_id;
// }
//
// template <typename T>
// // ID ThreadPool::add_job(const JobFunction &job_func, const T *func_data, Job *parent) {
// ID add_job(const JobFunction &job_func, const T *func_data, ID parent) {
//     auto job_id = _jobs.add(Job(job_func,func_data,parent));
//     _open_jobs_queue.push_back(job_id);
//     return job_id;
// }

// template <typename T>
// ID ThreadPool::add_open_job(const JobFunction &job_func, const T *func_data, Job *parent) {
//     auto job_id = _locked_jobs.add(job_func,func_data,parent,1);
//     // return &_locked_jobs.get(job_id);
//     return job_id;
// }
//

