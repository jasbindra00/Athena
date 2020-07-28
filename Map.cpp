#include "Map.h"
#include "FileReader.h"
#include "Log.h"
#include "KeyProcessing.h"

Map::Map(sf::RenderWindow* winptr){

}
using KeyProcessing::ToUpperString;
void Map::ReadStandardTiles(const std::string& tilefile) {
	FileReader file;
	if (!file.LoadFile(tilefile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the standard tile file of name " + tilefile);
		return;
	}
	file.NextLine();
	//reading tile dimensions
	for (int i = 0; i < 2; ++i) {
		auto strdimensions = KeyProcessing::ExtractAttributesToStream(file.GetWord(), true, true);
		auto attrtype = ToUpperString(strdimensions.GetWord());
		if (attrtype == "TILEDIMENSIONX") strdimensions >> tiledimension.x;
		else if (attrtype == "TILEDIMENSIONY") strdimensions >> tiledimension.y;
	}
	if (tiledimension.x == -1 || tiledimension.y == -1) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to read the tile dimensions from tile file of name " + tilefile);
		return;
	}
	while (!file.EndOfFile()) {
		file.NextLine();
		if (file.GetWord() != "TILE") { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify tile declaration on line number " + file.GetLineNumberString() + " in tile of filename " + tilefile); continue; }
		char tileid; {
			auto str = file.GetWord();
			if (str.size() > 1) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to read TILEID on line number " + file.GetLineNumberString() + " in tile file of name " + tilefile + ". ID must be a single character"); continue; }
			tileid = str[0];
			auto tileidexists = tiletextures.find(tileid);
			if (tileidexists != tiletextures.end()) { LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Tile with ID " + str + " from tile file of name + " + tilefile + " already exists"); continue; }
		}
		std::string attributetype; {
			auto keystream = KeyProcessing::ExtractAttributesToStream(file.GetWord(), true, false);
		}






		sf::Vector2f friction{ 0,0 };
		for (int i = 0; i < 2; ++i) {
			auto frictionstream = KeyProcessing::ExtractAttributesToStream(file.GetWord(), true, false);
			auto attributetype = frictionstream.GetWord();
			if (attributetype == "FRICTIONX") frictionstream >> friction.x;
			else if (attributetype == "FRICTIONY") frictionstream >> friction.y;
			else  LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to read friction values. Ensure that you stick to the order of the syntax as at the top of the file.");
		}

		
	}

	

}
void Map::LoadMap(const std::string& mapfile){
	FileReader file;
	if (!file.LoadFile(mapfile)) {
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the map file of name " + mapfile);
		return;
	}
	file.NextLine();

	
}

MapTile* Map::GetTile(const float& x, const float& y, const unsigned int& elevation) const
{
	return nullptr;
}

sf::Vector2f Map::GetTileSize() const
{
	return {};

}

void Map::Update(const float& dT)
{

}

void Map::Draw()
{

}
