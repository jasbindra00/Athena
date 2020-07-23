#ifndef COMPONENT_RETRIEVABLE_H
#define COMPONENT_RETRIEVABLE_H
#include "Component_Base.h"
class Component_Retrievable : public Component_Base {
protected:
public:
	Component_Retrievable() :Component_Base(ComponentType::RETRIEVABLE) {
		attributerequirement = 0;
	}
	virtual void ReadIn(std::stringstream& stream) override {

	}
	virtual ~Component_Retrievable() override{

	}
};
#endif