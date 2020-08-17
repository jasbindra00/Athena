#ifndef COMPONENT_SPRITE_H
#define COMPONENT_SPRITE_H
#include "Component_Drawable.h"
#include <SFML/Graphics.hpp>
#include "Manager_Texture.h"

using EntityID = unsigned int;
class Manager_Texture;
class Component_Sprite :public sf::Sprite, public Component_Drawable { //component for a basic drawable sprite. animations are in a seperate component
protected:
	std::string atlasmapname;
	std::shared_ptr<sf::Texture> atlasmap{ nullptr };
	Manager_Texture* texturemgr{ nullptr };
	using Component_Base::attributerequirement;
public:
	Component_Sprite() : Component_Drawable(ComponentType::SPRITE) {
		attributerequirement = 5;
	}
	void 
ureMgr(Manager_Texture* mgr) { texturemgr = mgr; }
	bool SetAtlasMap(const std::string& atlasmapname) {
		if (texturemgr == nullptr) {
			LOG::Log(LOCATION::COMPONENT_SPRITE, LOGTYPE::ERROR, __FUNCTION__, "Unable to request resource of name " + atlasmapname + " - texture manager pointer is not initialised.");
			return false;
		}
		atlasmap = texturemgr->RequestResource(atlasmapname);
		return (atlasmap == nullptr) ? false : true;
	}
	virtual void ReadIn(std::stringstream& stream) override {
		//ATTRIBUTES : ATLASMAPNAME LEFT TOP SIZEX SIZEY
		stream >> atlasmapname;
		SetAtlasMap(atlasmapname);
		sf::IntRect texturerect;
		stream >> texturerect.left >> texturerect.top >> texturerect.width >> texturerect.height;
		setTextureRect(std::move(texturerect));
	}
	virtual void Draw(sf::RenderTarget& target) const override {
		target.draw(static_cast<sf::Sprite>(*this));
	}
	using sf::Sprite::setTextureRect; //animation component support
	using sf::Sprite::setPosition; //move component  support
	using sf::Sprite::setScale; //transformable
	using sf::Sprite::setRotation; //transformable
	using sf::Sprite::getTextureRect;
	using sf::Sprite::getOrigin;
	using sf::Sprite::getPosition;
	using sf::Sprite::getScale;
	using sf::Sprite::getRotation;
	virtual ~Component_Sprite() override{

	}
};




#endif