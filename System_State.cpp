#include "System_State.h"
#include "ECSTypes.h"
#include "Utility.h"
#include "EntityMessages.h"
#include "Manager_Entity.h"
#include "Component_State.h"
#include "Manager_System.h"
#include "EntityDirection.h"
System_State::System_State(Manager_System* sysmgr, Manager_Entity* mgr, const SystemType& type):
	System_Base(sysmgr,mgr, type){
	requiredbitmask = Bitmask{ {Utility::ConvertToUnderlyingType(ComponentType::STATE)} };
	//messages are absolute truths.
	//events are consequences of these absolute truths.
/*	systemmgr->GetMessageHandler()->Subscribe(SystemMessage::MOVE, static_cast<SystemMailbox*>(this));*/
}

void System_State::ChangeEntityState(const EntityID& entityid, const EntityState& state){
	//when a state change occurs, other systems need to know 

}

void System_State::Update(const float& dt){

}
void System_State::SendToMailbox(const Message& msg){
	if (!ManagesEntity(msg.entityID)) return;
	switch (msg.messagetype) {
	case SystemMessage::CHANGE_STATE:{
		auto direction = static_cast<Direction>(msg.intdata);
		EntityEventType evnt;
		if (direction == Direction::UP) evnt = EntityEventType::MOVING_UP;
		else if (direction == Direction::RIGHT) evnt = EntityEventType::MOVING_RIGHT;
		else if (direction == Direction::DOWN) evnt = EntityEventType::MOVING_DOWN;
		else if (direction == Direction::LEFT) evnt = EntityEventType::MOVING_LEFT;
		//the change state change must be instant.
		ChangeEntityState(msg.entityID, EntityState::WALKING);
		//the movement changes must be sequential.
		systemmgr->AddEvent(msg.entityID, evnt);
	}
	}
}
void System_State::HandleEvent(const EntityID& id, const EntityEventType& evnt)
{

}
System_State::~System_State(){

}
