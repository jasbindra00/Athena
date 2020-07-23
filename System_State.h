#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H
#include "System_Base.h"
#include "EntityStates.h"
class System_State : public System_Base {
protected:
	using System_Base::requiredbitmask;
	using System_Base::managedentities;
	using System_Base::entitymanager;
	using System_Base::AddEntity;
	using System_Base::RemoveEntity;
	using System_Base::systemtype;
	using System_Base::systemmgr;
public:
	System_State(Manager_System* sysmgr, Manager_Entity* mgr, const SystemType& type);
	void ChangeEntityState(const EntityID& entityid, const EntityState& state);
	virtual void Update(const float& dt) override;
	virtual void SendToMailbox(const Message& msg) override;
	virtual void HandleEvent(const EntityID& id, const EntityEventType& evnt) override;
	virtual ~System_State();
};
#endif