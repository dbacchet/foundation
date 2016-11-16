#include "threadpool.h"

// local variables and functions
namespace {
}




BasicThreadPool::BasicThreadPool(unsigned int num_worker_threads) {
    // printf("size of Job struct: %lu\n", sizeof(Job));
    // printf("size of Job struct local_data: %lu\n", sizeof(Job::local_data));
    // printf("thread id: %llu\n", thread_id());
    // create the worker threads
    for (auto i=0; i<num_worker_threads; i++) {
        // _workers.push_back(std::thread(basic_worker_thread_func, this));
        _workers.push_back(std::thread(&BasicThreadPool::worker_thread_function,this));
    }
}

BasicThreadPool::~BasicThreadPool() {
    stop();
    _cv.notify_all();
    for (auto&& w: _workers) {
        w.join();
    }
    _workers.clear();
}

// add a job to the pool
ID BasicThreadPool::add_job(const Job &job) {
    ID job_id;
    {
        std::unique_lock<std::mutex> lock(_mutex);
        job_id = _jobs.add(job);
        _open_jobs_queue.push_back(job_id);
        // increase unfinished job count in the parent
        if (valid(job.parent_id) && _jobs.has(job.parent_id)) {
            _jobs.get(job.parent_id).unfinished_jobs++;
        }
    }
    _cv.notify_one();
    return job_id;
}

bool BasicThreadPool::get_next_job(Job &job) {
    std::unique_lock<std::mutex> lock(_mutex);
    return get_next_job_unsafe(job);
}

bool BasicThreadPool::get_next_job_unsafe(Job &job) {
    auto job_id = _open_jobs_queue.front();
    _open_jobs_queue.pop_front();
    if (_jobs.has(job_id)) {
        job = _jobs.get(job_id);
        _jobs.remove(job_id);
        return true;
    }
    return false;
}

// start the workers
void BasicThreadPool::start() {
    std::unique_lock<std::mutex> lock(_mutex);
    _state = ThreadPoolState::ACTIVE;
    _cv.notify_all();
}

// wait until all workers complete
void BasicThreadPool::wait() {
    /// \todo replace the wait spin with a condition variable
    while (_state!=ThreadPoolState::STOPPED && _jobs.size()>0)
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

// stop the execution
void BasicThreadPool::stop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _state = ThreadPoolState::STOPPED;
}

// clear the queue of open jobs;
void BasicThreadPool::clear() {

}


void BasicThreadPool::worker_thread_function() {
    while (_state!=ThreadPoolState::STOPPED) {
        Job job;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [&]{ return _state==ThreadPoolState::STOPPED
                    || (_state==ThreadPoolState::ACTIVE && !_open_jobs_queue.empty()); });
            // if stopped, exit
            if (_state==ThreadPoolState::STOPPED) {
                // printf("stop!!! (thread %llu)\n",thread_id());
                return;
            }
            // here we are guaranteed that there is at least a job in the queue
            if (!get_next_job_unsafe(job)) {
                // printf("no more open jobs!!! (thread %llu)\n",thread_id());
                return;
            }
        }
        // printf("execute fun in thread %llu\n",thread_id());
        if (job.function)
            job.function(&job);
    }
}





ThreadPool::ThreadPool(unsigned int num_worker_threads)
: BasicThreadPool(0) {
    // create the worker threads
    for (auto i=0; i<num_worker_threads; i++) {
        _workers.push_back(std::thread(&ThreadPool::worker_thread_function,this));
    }
}

ThreadPool::~ThreadPool() {
}

ID ThreadPool::add_locked_job(const Job &job) {
    std::unique_lock<std::mutex> lock(_mutex);
    auto id = _jobs.add(job);
    _jobs.get(id).unfinished_jobs++;
    _locked_jobs.insert(id);
    return id;
}

void ThreadPool::unlock_job(ID job_id) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_jobs.has(job_id))
        _jobs.get(job_id).unfinished_jobs--;
}

void ThreadPool::wait() {
    /// \todo replace the wait spin with a condition variable
    while (_state!=ThreadPoolState::STOPPED && _jobs.size()>0) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    }
}

void ThreadPool::worker_thread_function() {
    while (_state!=ThreadPoolState::STOPPED) {
        Job job;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _cv.wait(lock, [&]{ return _state==ThreadPoolState::STOPPED
                    || (_state==ThreadPoolState::ACTIVE && !_open_jobs_queue.empty()); });
            // if stopped, exit
            if (_state==ThreadPoolState::STOPPED) {
                // printf("stop!!! (thread %llu)\n",thread_id());
                return;
            }
            // here we are guaranteed that there is at least a job in the queue
            if (!get_next_job_unsafe(job)) {
                // printf("no more open jobs!!! (thread %llu)\n",thread_id());
                return;
            }
        }
        // printf("execute fun in thread %llu\n",thread_id());
        if (job.function)
            job.function(&job);
        // notify the parent job
        if (valid(job.parent_id)) {
            std::unique_lock<std::mutex> lock(_mutex);
            if (_jobs.has(job.parent_id)) {
                Job &j = _jobs.get(job.parent_id);
                j.unfinished_jobs--;
                if (j.unfinished_jobs<=0) {
                    _locked_jobs.erase(job.parent_id);
                    _open_jobs_queue.push_back(job.parent_id);
                }
            }
        }
    }
}
