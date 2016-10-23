// Taken from http://stackoverflow.com/a/16075550
#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class SafeQueue {
public:
	SafeQueue() :
			q(), m(), c() {
	}

	virtual ~SafeQueue() = default;

	void enqueue(T&& t) {
		std::lock_guard<std::mutex> lock(m);
		q.push(std::move(t));
		c.notify_one();
	}

	T dequeue() {
		std::unique_lock<std::mutex> lock(m);
		while (q.empty()) {
			c.wait(lock);
		}
		T val = q.front();
		q.pop();
		return val;
	}

private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};
