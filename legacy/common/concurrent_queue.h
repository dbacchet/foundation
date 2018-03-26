#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

/// \class ConcurrentQueue
/// concurrent queue using condition variables to allow consumers to sleep until there are messages in the queue
template <typename T>
class ConcurrentQueue final
{
public:

    ConcurrentQueue() {}
    ConcurrentQueue( const ConcurrentQueue &cq) = delete;
    ~ConcurrentQueue() {}

    /// pop front item. If the queue is empty, the caller is suspended until new items are pushed.
    T pop() {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty()) {
            _cv.wait(mlock);
        }
        auto item = _queue.front();
        _queue.pop();
        return item;
    }

    /// pop filling the given item. If the queue is empty, the caller is suspended until new items are pushed.
    void pop(T& item) {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty()) {
            _cv.wait(mlock);
        }
        item = _queue.front();
        _queue.pop();
    }

    /// fill the given item and pop; if queue is empty return false immediately
    bool pop_async(T& item) {
        std::unique_lock<std::mutex> mlock(_mutex);
        if (_queue.empty())
            return false;
        item = _queue.front();
        _queue.pop();
        return true;
    }

    /// push a copy of the given item
    void push(const T& item) {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push(item);
        mlock.unlock();
        _cv.notify_one();
    }

    /// push using move semantics
    void push(T&& item) {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push(std::move(item));
        mlock.unlock();
        _cv.notify_one();
    }

    size_t size() {
        std::unique_lock<std::mutex> mlock(_mutex);
        return _queue.size();
    }

private:

    std::queue<T>           _queue;
    std::mutex              _mutex;
    std::condition_variable _cv;
};

