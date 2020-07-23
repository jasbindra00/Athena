#ifndef COMPONENT_DRAWABLE_H
#define COMPONENT_DRAWABLE_H
#include "Component_Base.h"

class Component_Drawable : public Component_Base { //abstract intermediate for Component_Sprite
protected:
public:
	Component_Drawable(const ComponentType& type) : Component_Base(type) {
	}
	virtual void ReadIn(std::stringstream& stream) = 0;
	virtual void Draw(sf::RenderTarget& target) const = 0;
	virtual ~Component_Drawable() override {

	}
};



#endif