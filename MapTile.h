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
		friend Attributes& operator>>(Attributes& stream, StaticTile* tile) {
			std::string errorstring;
			while (!stream.eof()) {
				auto pos = stream.tellg();
				Attributes keystream = KeyProcessing::ExtractAttributesToStream(stream.GetWord());
				std::string attributetype = keystream.GetWord();
				if (attributetype == "FRICTIONX") keystream>>tile->friction.x;
				else if (attributetype == "FRICTIONY") keystream >> tile->friction.y;
				else if (attributetype == "TOPLEFTX") keystream >> tile->texturerect.left;
				else if (attributetype == "TOPLEFTY") keystream >> tile->texturerect.top;
				else if (attributetype == "TEXTURESIZEX") keystream >> tile->texturerect.width;
				else if (attributetype == "TEXTURESIZEY") keystream >> tile->texturerect.height;
				else if (attributetype == "SPRITESHEET") {
					if (!tile->SetTexture(keystream.GetWord())) errorstring.append("SPRITESHEET ");
				}
			}
			if (tile->texturerect.width <= 0 || tile->texturerect.height <= 0) errorstring += "TEXTURERECT";
			throw CustomException(errorstring);
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