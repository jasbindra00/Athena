#ifndef SYSTEM_BASE_H
#define SYSTEM_BASE_H
#include <unordered_set>
#include "SystemMailbox.h"
#include "Bitmask.h"
#include "ECSTypes.h"
using EntityID = unsigned int;
using ManagedEntities = std::unordered_set<EntityID>;
class Manager_Entity;
class Manager_System;
class System_Base : public SystemMailbox{
	friend class Manager_System;
protected:
	Bitmask requiredbitmask; //the bitmask needed for an entity to be managed by this system.
	ManagedEntities managedentities; //all the entities that are managed by this system.
	Manager_Entity* entitymanager;
	Manager_System* systemmgr;
	SystemType systemtype;
	void AddEntity(const EntityID& id, const Bitmask& b) { 
		managedentities.insert(id);
	}
	bool RemoveEntity(const EntityID& id) {
		auto foundentity = managedentities.find(id);
		if (foundentity != managedentities.end()) managedentities.erase(foundentity); return true;
		return false;
	}
	auto FindEntity(const EntityID& id)->ManagedEntities::iterator {
		return managedentities.find(id);
	}
public:
	System_Base(Manager_System* sysmgr, Manager_Entity* mgr, const SystemType& systype):systemmgr(sysmgr),entitymanager(mgr), systemtype(systype) {
	}
	inline bool ManagesEntity(const EntityID& id) const {
		auto found = managedentities.find(id);
		if (found == managedentities.end()) return false;
		return true;
	}
	inline bool MatchesRequirements(const Bitmask& b) const {
		return requiredbitmask.Matches(b);
	}
	virtual void Update(const float& dT) = 0;
	virtual void SendToMailbox(const Message& msg) override = 0;
	virtual void HandleEvent(const EntityID& id, const EntityEventType& evnt) = 0;
	virtual ~System_Base() override {
	}
};

#endif