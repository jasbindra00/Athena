#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H
#include <queue>

template<typename T>
class EventQueue {
protected:
	std::queue<T> eventqueue;
public:
	bool PollEvent(T& arg) {
		if (eventqueue.empty()) return false;
		arg = eventqueue.front();
		eventqueue.pop();
		return true;
	}
	void InsertEvent(const T& evnt) {
		eventqueue.emplace(evnt);
	}
};


#endif