#ifndef MAPTILE_H
#define MAPTILE_H
#include <string>
#include <SFML/Graphics.hpp>
#include "Manager_Texture.h"
#include "StreamAttributes.h"
#include "EnumConverter.h"
#include "KeyProcessing.h"
#include "CustomException.h"
#include <iostream>

namespace TileData {
	class StaticTile {
	private:
		std::string atlasmapname{ "" };
		sf::Sprite tilesprite;
		sf::Vector2f friction{ -1,-1 };
		Manager_Texture* texturemgr;
		mutable sf::IntRect texturerect;
	public:
		StaticTile(Manager_Texture* mgr) :texturemgr(mgr) {
			texturerect.width = 0;
			texturerect.height = 0;
		}
		sf::IntRect& GetTextureRect() const { return texturerect; }
		sf::Sprite& GetTileSprite() { return tilesprite; }
		bool SetTexture(const std::string& atlas) {
			SharedTexture texture = texturemgr->RequestResource(atlas);
			if (texture == nullptr) {
				texture = texturemgr->RequestResource("ERRORTILE");
				atlasmapname = std::string{ "ERRORTILE" };
				return false;
			}
			tilesprite.setTexture(*texture); //shared ptr, exists in texturemgr.
			return true;
		}
		virtual ~StaticTile() {
			if (!atlasmapname.empty()) {
				texturemgr->RequestResourceDealloc(atlasmapname);
			}
		}
		void InitTile(const KeyProcessing::Keys& keys) {
			Attributes keyvalues = KeyProcessing::DistillValuesToStream(keys, '0');
			std::string errorstring{ "" };
			keyvalues >> texturerect.width >> texturerect.height >> texturerect.left >> texturerect.top >> friction.x >> friction.y >> atlasmapname;
			if (texturerect.width <= 0 || texturerect.height <= 0 || !texturemgr->RequestResource(atlasmapname)) errorstring += "TEXTURERECT ";
			if (!errorstring.empty()) throw CustomException(std::move(errorstring));
		}
	};
	struct MapTile {
		char statictileid{ '0' };
		bool teleport{ false };
		bool deadly{ false };
		int layer{ 0 };
	};
}

#endif