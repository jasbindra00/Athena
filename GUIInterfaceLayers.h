#ifndef GUIINTERFACELAYERS_H
#define GUIINTERFACELAYERS_H
#include <SFML\Graphics\Sprite.hpp>
#include <SFML\Graphics\RenderTexture.hpp>
class GUIInterfaceLayers {

protected:
	sf::Sprite backgroundsprite;
	sf::RenderTexture backgroundlayer;

	sf::Sprite contentsprite;
	sf::RenderTexture contentlayer;

	sf::Sprite controlsprite;
	sf::RenderTexture controllayer;

public:
	GUIInterfaceLayers(const sf::Vector2f& eltsize) {
		backgroundlayer.create(eltsize.x, eltsize.y);
		backgroundsprite.setTexture(backgroundlayer.getTexture());

		contentlayer.create(eltsize.x, eltsize.y);
		contentsprite.setTexture(contentlayer.getTexture());

		controllayer.create(eltsize.x, eltsize.y);
		controlsprite.setTexture(controllayer.getTexture());
	}

	void ResetLayerSize(const sf::Vector2f& eltsize) {
;
		backgroundlayer.create(eltsize.x, eltsize.y);
		backgroundsprite.setTexture(backgroundlayer.getTexture());

		contentlayer.create(eltsize.x, eltsize.y);
		contentsprite.setTexture(contentlayer.getTexture());

		controllayer.create(eltsize.x, eltsize.y);
		controlsprite.setTexture(controllayer.getTexture());
		
	}
	sf::RenderTexture* GetControlLayer() { return &controllayer; }
	sf::RenderTexture* GetBackgroundLayer() { return &backgroundlayer; }
	sf::RenderTexture* GetContentLayer() { return &contentlayer; }

	sf::Sprite* GetBackgroundSprite() { return &backgroundsprite; }
	sf::Sprite* GetContentSprite() { return &contentsprite; }
	sf::Sprite* GetControlSprite() { return &controlsprite; }


};




#endif