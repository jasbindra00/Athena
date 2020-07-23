#ifndef COMPONENT_STATE_H
#define COMPONENT_STATE_H
#include "Component_Base.h"
#include "EntityStates.h"


//the movement system is dependent on the state system.
class Component_State : public Component_Base
{
protected:
	EntityState currentstate;
public:
	Component_State() : Component_Base(ComponentType::STATE) { attributerequirement = 1; }
	inline EntityState GetState() const noexcept { return currentstate; }
	inline void ChangeState(const EntityState& state) { currentstate = state; }
	virtual void ReadIn(std::stringstream& stream) override {
		unsigned int statetype;
		stream >> statetype;
		currentstate = static_cast<EntityState>(currentstate);
	}
	
};




#endif