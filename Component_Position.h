#ifndef COMPONENT_POSITION_H
#define COMPONENT_POSITION_H
#include "Component_Base.h"
#include <SFML\Graphics\Transform.hpp>
//component for manipulating the position and elevation of a component.
class Component_Position : public Component_Base {
protected:
	sf::Vector2f position;
	//old position?
	unsigned int elevation;
public:
	Component_Position() :Component_Base(ComponentType::POSITION), elevation{ 0 }, position{ 0,0 } {
		attributerequirement = 3;
	}
	void SetPosition(const sf::Vector2f& pos) { position = pos; }
	void SetElevation(const unsigned int& elevationnew) { elevation = elevationnew; }

	void Move(const sf::Vector2f& offset) { position += offset; }

	void IncrementElevation() { ++elevation; }
	void DecrementElevation() { (elevation == 0) ? elevation : --elevation; }

	unsigned int GetElevation() const { return elevation; }
	sf::Vector2f GetPosition() const { return position; }

	virtual void ReadIn(std::stringstream& stream) override {
		//ATTRIBUTES : positionx positiony elevation
		stream >> position.x >> position.y >> elevation; 
	}
	~Component_Position() override {
	}
};




#endif