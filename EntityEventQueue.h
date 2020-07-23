#ifndef ENTITYEVENTQUEUE_H
#define ENTITYEVENTQUEUE_H
#include <queue>
#include "ECSTypes.h"
class EntityEventQueue{
protected:
	std::queue<EntityEventType> eventqueue;
public:
	bool PollEvent(EntityEventType& evnt) {
		if (eventqueue.empty()) return false;
		evnt = eventqueue.front();
		eventqueue.pop();
	}
	void InsertEvent(const EntityEventType& evnt) {
		eventqueue.push(evnt);
	}
};



#endif 
