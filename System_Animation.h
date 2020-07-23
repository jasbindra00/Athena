#ifndef SYSTEM_ANIMATION_H
#define SYSTEM_ANIMATION_H
#include "System_Base.h"

class System_Animation : public System_Base{
protected:
	using System_Base::requiredbitmask;
	using System_Base::managedentities;
	using System_Base::entitymanager;
	using System_Base::AddEntity;
	using System_Base::RemoveEntity;
public:
	System_Animation(Manager_System* sysmgr, Manager_Entity* mgr, const SystemType& type);
	virtual void Update(const float& dt) override;
	virtual void SendToMailbox(const Message& msg) override;
	virtual void HandleEvent(const EntityID& id, const EntityEventType& evnt) override;
	virtual ~System_Animation();
};


#endif
