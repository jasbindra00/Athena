#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H
#include <queue>


/*
-Inserting a processed change here will be difficult to reset.
*/
template<typename T>
class CustomQueue {
protected:
	std::queue<T> eventqueue;
public:
	void MyFunction()
	{
		
	}
	bool Poll(T& arg) {
		if (eventqueue.empty()) return false;
		arg = eventqueue.front();
		eventqueue.pop();
		return true;
	}
	void Insert(const T& evnt) {
		eventqueue.emplace(evnt);
	}
	std::size_t GetSize()
	{
		return eventqueue.size();
	}
	
};


#endif