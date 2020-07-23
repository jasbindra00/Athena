#include "System_Animation.h"
#include "Manager_System.h"


System_Animation::System_Animation(Manager_System* sysmgr, Manager_Entity* mgr, const SystemType& type) :System_Base(sysmgr, mgr, type){

}

void System_Animation::Update(const float& dt)
{

}

void System_Animation::SendToMailbox(const Message& msg)
{

}

void System_Animation::HandleEvent(const EntityID& id, const EntityEventType& evnt)
{

}

System_Animation::~System_Animation()
{

}
