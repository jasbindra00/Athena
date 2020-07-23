#ifndef COMPONENT_COLLISIONAABB_H
#define COMPONENT_COLLISIONAABB_H
#include "Component_Base.h"
#include "Origin.h"
#include <SFML\Graphics\Transform.hpp>
//component to manipulate the bounding box of an entity.


class Component_CollisionAABB : public Component_Base {
protected:
	OriginX xcentre;  //9 origin position permutations across the bounding box
	OriginY ycentre;
	sf::FloatRect AABB;
	sf::Vector2f origincoords;
	sf::Vector2f originoffset;
	bool collidingonX;
	bool collidingonY;
public:
	Component_CollisionAABB() :Component_Base(ComponentType::COLLIDABLE), xcentre(OriginX::LEFT), ycentre(OriginY::TOP), collidingonX(false), collidingonY(false) {
		attributerequirement = 8;
	}
	inline sf::Vector2f GetOriginCoords() const noexcept { return origincoords; }
	inline OriginX GetOriginXPos() const noexcept { return xcentre; };
	inline OriginY GetOriginYPos() const noexcept { return ycentre; };
	inline sf::FloatRect GetBoundingBox() const noexcept { return AABB; }
	inline bool GetCollidingOnX() const noexcept { return collidingonX; }
	inline bool GetCollidingOnY() const noexcept { return collidingonY; }

	inline void SetAABB(const sf::FloatRect& rect) { AABB = rect; }
	inline void SetSize(const sf::Vector2f& vec) {
		AABB.width = vec.x;
		AABB.height = vec.y;
		UpdatePosition(origincoords);
	}
	inline void SetOrigin(const OriginX& x, const OriginY& y) { xcentre = x; ycentre = y; }
	inline void SetOffset(const sf::Vector2f& offset) { originoffset = offset;}
	inline void ResetCollisionFlags() {
		collidingonX = false; collidingonY = false;
	}
	inline void SetCollidingOnX(const bool& state) { collidingonX = state; }
	inline void SetCollidingOnY(const bool& state) { collidingonY = state; }
	inline void UpdatePosition(const sf::Vector2f& position) { //updates the position of the AABB, ensuring that the centre of the box is at the custom origin.
		if (xcentre == OriginX::LEFT) { AABB.left = position.x + originoffset.x;
			if (ycentre == OriginY::TOP) { AABB.top = position.y; }
			else if (ycentre == OriginY::MIDDLE) { AABB.top = position.y - AABB.height / 2 + originoffset.y; }
			else if (ycentre == OriginY::BOT) { AABB.top = position.y - AABB.height + originoffset.y; }
		}
		else if (xcentre == OriginX::MIDDLE) { AABB.left = position.x - AABB.width / 2 + originoffset.x;
			if (ycentre == OriginY::TOP) { AABB.top = position.y + originoffset.y;}
			else if (ycentre == OriginY::MIDDLE) { AABB.top = position.y - AABB.height / 2 + originoffset.y;}
			else if (ycentre == OriginY::BOT) {	AABB.top = position.y - AABB.height + originoffset.y;}
		}
		else if (xcentre == OriginX::RIGHT) { AABB.left = position.x - AABB.width + originoffset.x;
			if (ycentre == OriginY::TOP) { AABB.top = position.y + originoffset.y;}
			else if (ycentre == OriginY::MIDDLE) {	AABB.top = position.y - AABB.height / 2 + originoffset.y;}
			else if (ycentre == OriginY::BOT) { AABB.top = position.y - AABB.height + originoffset.y;}
		}
		origincoords = position + originoffset;
	}
	inline bool Contains(const sf::Vector2f& vec) const noexcept { return AABB.contains(vec); }
	inline bool Intersects(const sf::FloatRect& other) const noexcept {	return AABB.intersects(other);}
	inline bool Intersects(const sf::IntRect& other) const noexcept { return AABB.intersects(sf::FloatRect{ sf::Vector2f{static_cast<float>(other.left), static_cast<float>(other.top)}, sf::Vector2f{static_cast<float>(other.width), static_cast<float>(other.height)} }); }

	virtual void ReadIn(std::stringstream& stream) override {
		//ATTRIBUTES : ORIGINX ORIGIN Y TLX TLY SIZEX SIZEY OFFSETX OFFSETY
		unsigned int originx;
		unsigned int originy;
		stream >> originx >> originy >> AABB.left >> AABB.top >> AABB.width >> AABB.height >> originoffset.x >> originoffset.y;
		SetOrigin(static_cast<OriginX>(originx), static_cast<OriginY>(originy));	
	}
	virtual ~Component_CollisionAABB() {
	}
};


#endif