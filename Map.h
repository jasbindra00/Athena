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
#include "CustomException.h"
#include <assert.h>
#include <functional>
namespace MapData {
	using namespace TileData;
	using StaticTilePtr = std::unique_ptr<StaticTile>;
	using StaticTiles = std::unordered_map<char, StaticTilePtr>;
	using MapTiles = std::vector<std::vector<MapTile>>;
	using ConfigurationReader = std::function<bool(std::string)>;
	enum class ConfigurationType {
		GAME_MAP, 
	};
}
class Map {
private:
	
	static std::unordered_map<



	MapData::StaticTiles statictiles;
	MapData::MapTiles maptiles;
	Manager_Texture* texturemgr;
	sf::Vector2i tiledimension;
	sf::Vector2i mapdimension{ 0,0 };
	
	void ReadConfiguration(const std::string& configurationtype, const std::string& configuration);
	bool ReadStandardTiles(const std::string& tilefile);
	void LoadMap(const std::string& mapfile);


public:
	Map(Manager_Texture* mgr); 

};
#endif
 