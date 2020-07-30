#ifndef MAP_H
#define MAP_H
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include "FileReader.h"
#include "Log.h"
#include "KeyProcessing.h"
#include "Manager_Texture.h"
#include "MapTile.h"
namespace MapData {
	using namespace TileData;
	using StaticTilePtr = std::unique_ptr<StaticTile>;
	using StaticTiles = std::unordered_map<char, StaticTilePtr>;
	using MapTiles = std::vector<std::vector<MapTile>>;
}
class Map {
private:
	MapData::StaticTiles statictiles;
	MapData::MapTiles maptiles;
	Manager_Texture* texturemgr;
	sf::Vector2i tiledimension;
	void ReadStandardTiles(const std::string& tilefile) {
		using KeyProcessing::ToUpperString;
		using KeyProcessing::ExtractAttributesToStream;
		using namespace TileData;
		FileReader file;
		if (!file.LoadFile(tilefile)) {
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the standard tile file of name " + tilefile);
			return;
		}
		std::string DidNotReadTile{ " DID NOT READ TILE..." };
		std::string TileFileStr{ " in tile file of name " + tilefile };
		file.NextLine();
		for (int i = 0; i < 2; ++i) { //we must read the tile pixel dimensions before anything else.
			auto dimensionstream = KeyProcessing::ExtractAttributesToStream(file.GetWord());
			auto attrtype = ToUpperString(dimensionstream.GetWord());
			if (attrtype == "TILEDIMENSIONX") dimensionstream >> tiledimension.x;
			else if (attrtype == "TILEDIMENSIONY") dimensionstream >> tiledimension.y;
		}
		if (tiledimension.x <= 0 || tiledimension.y <= 0) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Invalid tile dimensions in tile file of name " + tilefile); return; }
		auto tileattributes = static_cast<Attributes*>(&file.GetLineStream());
		while (!file.EndOfFile()) {
			file.NextLine();
			auto statictileobj = std::make_unique<StaticTile>(texturemgr);
			char tileid = '~';
			Attributes tileidstream(ExtractAttributesToStream(tileattributes->GetWord()));
			try {//checking plausiblity of the TILEID.
				if (ToUpperString(tileidstream.GetWord()) != "TILEID") throw std::string{ "Unable to find the TILEID for standard tile on line " + file.GetLineNumberString() + "." };
				else {
					std::string tileidstr = tileidstream.GetWord();
					if (tileidstr.size() > 1) throw std::string{ "The char typedef for a static tile should be a single character on " + file.GetLineNumberString() + "." };
					else if (tileidstr[0] == '~') throw std::string{ "Invalid tile char typedef - '~' is a reserved char." };
					else if (statictiles.find(tileidstr[0]) != statictiles.end()) {
						throw std::string{ "A static tile with TILEID '" + tileidstr + "' already exists." };
					}
					tileid = tileidstr[0];
				}
			}
			catch (const std::string& errorstring) { //any problems with the tile id will skip the tile completely
				LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, errorstring + TileFileStr + DidNotReadTile);
				statictileobj.reset();
				continue;
			}
			try { *tileattributes >> statictileobj.get(); } //read the remainder of the attributes into the object.
			catch (const std::string& errorstring) {
				if (!errorstring.empty()) {
					Attributes errorstream(errorstring);
					while (!errorstream.eof()) {
						std::string errortype = errorstream.GetWord();
						if (errortype == "SPRITESHEET") LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify spritesheet for tile of ID " + std::string{ tileid } + TileFileStr + ". ERROR TEXTURE SET...");
						else if (errortype == "TEXTURERECT") {
							LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Tile of ID " + std::string{ tileid } + TileFileStr + " has invalid texture dimensions." + DidNotReadTile);
							statictileobj->GetTextureRect().width = 32; // set to error texture dimensions
							statictileobj->GetTextureRect().height = 32;
						}
					}
				}
			}
			//scale the sprite texture to match the specified tile pixel dimensions.
			sf::Sprite& tilesprite = statictileobj->GetTileSprite();
			tilesprite.setTextureRect(statictileobj->GetTextureRect());
			tilesprite.setScale(tiledimension.x / tilesprite.getTextureRect().width, tiledimension.y / tilesprite.getTextureRect().height); //scale the texture down to tile pixel dimension
			statictiles[tileid] = std::move(statictileobj);
		}
	}

public:
	Map(Manager_Texture* mgr):texturemgr(mgr){
		ReadStandardTiles("StandardTiles.txt");
	}

};
#endif
 