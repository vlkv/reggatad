// Taken from http://stackoverflow.com/a/16075550
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <boost/thread.hpp>

template<class T>
class SafeQueue {
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;

public:
	SafeQueue() : q(), m(), c() {}
	virtual ~SafeQueue() = default;

	void enqueue(T&& t);
	T dequeue();

private:
	T frontPop();
};


template<class T>
void SafeQueue<T>::enqueue(T&& t) {
	std::lock_guard<std::mutex> lock(m);
	q.push(std::move(t));
	c.notify_one();
}

template<class T>
T SafeQueue<T>::dequeue() {
	std::unique_lock<std::mutex> lock(m);
	while (q.empty()) {
		c.wait_for(lock, std::chrono::milliseconds(25));
		boost::this_thread::interruption_point();
	}
	return frontPop();
}

template<class T>
T SafeQueue<T>::frontPop() {
	T val = std::move(q.front());
	q.pop();
	return val;
}

