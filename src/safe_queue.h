// Taken from http://stackoverflow.com/a/16075550
#pragma once
#include <boost/thread.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>


template<class T>
class SafeQueue {
    std::queue<T> _queue;
    mutable std::mutex _mutex;
    std::condition_variable _condVar;

public:
    SafeQueue() : _queue(), _mutex(), _condVar() {
    }
    virtual ~SafeQueue() = default;

    void enqueue(T&& t);
    T dequeue();

private:
    T frontPop();
};

template<class T>
void SafeQueue<T>::enqueue(T&& t) {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(std::move(t));
    _condVar.notify_one();
}

template<class T>
T SafeQueue<T>::dequeue() {
    std::unique_lock<std::mutex> lock(_mutex);
    while (_queue.empty()) {
        _condVar.wait_for(lock, std::chrono::milliseconds(25));
        boost::this_thread::interruption_point();
    }
    return frontPop();
}

template<class T>
T SafeQueue<T>::frontPop() {
    T val = std::move(_queue.front());
    _queue.pop();
    return val;
}

