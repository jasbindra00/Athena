#ifndef COMPONENT_MOTION_H
#define COMPONENT_MOTION_H
#include "Component_Base.h"
#include "EntityDirection.h"
#include <SFML\Graphics\Transform.hpp>
enum class Axis
{
	X, Y
};

class Component_Motion : public Component_Base {//component for velocity/acceleration functionality and manipulation.
protected:
	sf::Vector2f velocity;
	sf::Vector2f movementimpulse; //scalar impulse added to velocity from change in direction
	sf::Vector2f acceleration;
	float maxvelocity{ 0 };
	float maxacceleration{ 0 };
public:
	Component_Motion():Component_Base(ComponentType::MOTION) {
		attributerequirement = 2;
	}
	inline void AddAcceleration(const sf::Vector2f& a) { acceleration += a; }
	inline void SetVelocity(const sf::Vector2f& v) { velocity = v; }
	inline void KillVelocity(const Axis& axis) { (axis == Axis::X) ? velocity.x = 0 : velocity.y = 0; }
	inline void SetMaxVelocity(const float& maxvel) { maxvelocity = maxvel; }
	inline void SetMaxAcceleration(const float& maxaccel) { maxacceleration = maxaccel; }
	inline void SetMovementStep(const sf::Vector2f& step) { movementimpulse = step; }

	void AddVelocity(const sf::Vector2f& acceleration) {
		velocity += acceleration;
		LimitVelocity(velocity, maxvelocity);
	}
	float GetMaxAcceleration() const { return maxacceleration; }
	float GetMaxVelocity() const { return maxvelocity; }
	sf::Vector2f GetVelocity() const { return velocity; }
	sf::Vector2f GetAcceleration() const { return acceleration; }
	void ApplyScalarFriction(const sf::Vector2f& scalarfriction) {
		//the input is the scalar friction velocity that opposes our velocity.
		//since it is a scalar, we must find the direction in which to apply it.
		if(velocity.x != 0){
			if (abs(velocity.x) - abs(scalarfriction.x) < 0) { //if the friction changes our direction.
				velocity.x = 0;
			}
			else {
				velocity.x = velocity.x - ((velocity.x < 0) ? abs(scalarfriction.x) : -abs(scalarfriction.x));
			}
		}
		if (velocity.y != 0) {
			if (abs(velocity.y) - abs(scalarfriction.y) < 0) { 
				velocity.y = 0;
			}
			else { //otherwise
				velocity.y +=((velocity.y > 0) ? -abs(scalarfriction.y) : abs(scalarfriction.y)); //apply friction to the opposite direction of our velocity.
			}
		}
	}
	void Move(const Direction& dir) { //applies acceleration in that direction.
		if (dir == Direction::UP) { acceleration.y -= movementimpulse.y; }
		else if (dir == Direction::RIGHT) { acceleration.x += movementimpulse.x; }
		else if (dir == Direction::DOWN) { acceleration.y += movementimpulse.y; }
		else if (dir == Direction::LEFT) { acceleration.x -= movementimpulse.x; }
	}
	static float VectorSqMagnitude(const sf::Vector2f& v) { return pow(v.x, 2) + pow(v.y, 2); }
	static void LimitVelocity(sf::Vector2f& v, const float& max) {
		if (VectorSqMagnitude(v) < max * max) return;
		v /= sqrt(VectorSqMagnitude(v));
		v *= max;
	}
	void ReadIn(std::stringstream& stream) override {
		//ATTRIBUTES : maxvelocity max acceleration
		stream >> maxvelocity >> maxacceleration; 
	}
	virtual ~Component_Motion() override {
	}
};



#endif