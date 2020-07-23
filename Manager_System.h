#ifndef MANAGER_SYSTEM_H
#define MANAGER_SYSTEM_H
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <memory>
#include "ECSTypes.h"
#include "EntityEventQueue.h"
#include "MessageHandler.h"
#include "System_Base.h"
#include "Bitmask.h"
using EntityID = unsigned int;
class System_Base;
class MessageHandler;
using SystemPtr = std::unique_ptr<System_Base>;
using EntityEvents = std::unordered_map<EntityID, EntityEventQueue>;
using SystemStorage = std::unordered_map<SystemType, SystemPtr>;

class Manager_System{
protected:
	std::unique_ptr<MessageHandler> messagehandler;
	EntityEvents entityevents;
	SystemStorage systemstorage;
	template<typename T>
	T* GetSystem(const SystemType& systype) const {
		auto foundsys = systemstorage.find(systype);
		return (foundsys == systemstorage.end()) ? nullptr : dynamic_cast<T*>(foundsys->second);
	}
public:
	Manager_System();
	void HandleEvents(const float& dT); //responsible for distributing entity events to those systems that manage it.
	void DelegateEntity(const EntityID& id, const Bitmask& b); //responsible for assigning entity to relevant systems.
	void ChangedEntity(const EntityID& id, const Bitmask& b); //if an entity bitmask has changed, it may not be supported by its current systems anymore.
	void AddEvent(const EntityID& id, const EntityEventType& type); //adds event to the entity event queue.
	void RemoveEntity(const EntityID& id);
	MessageHandler* GetMessageHandler() const noexcept;

};
#endif