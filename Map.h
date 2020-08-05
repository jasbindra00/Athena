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
#include "EnumConverter.h"
#include <assert.h>
#include <functional>
#include <type_traits>
#include <unordered_set>
namespace MapData {
	using namespace TileData;
	using StaticTilePtr = std::unique_ptr<StaticTile>;
	using StaticTiles = std::unordered_map<char, StaticTilePtr>;
	using MapTiles = std::vector<std::vector<MapTile>>;
	
	namespace ConfigurationData {
		using ConfigurationReader = std::function<bool(std::string, int)>;
		static enum class ConfigurationType {
			GAME, LAYER, TELEPORT, DEADLY, NULLTYPE
		};
		static EnumConverter<ConfigurationType> configurationconverter([](const std::string& str)->ConfigurationType {
			if (str == "GAME_MAP") return ConfigurationType::GAME;
			else if (str == "LAYER_MAP") return ConfigurationType::LAYER;
			else if (str == "TELEPORT_MAP") return ConfigurationType::TELEPORT;
			else if (str == "DEADLY_MAP") return ConfigurationType::DEADLY;
			return ConfigurationType::NULLTYPE;
			});
	}
}
using namespace MapData::ConfigurationData;
class Map {
private:
	std::unordered_map < ConfigurationType, ConfigurationReader> configurationreaders;
	MapData::StaticTiles statictiles;
	MapData::MapTiles maptiles;
	Manager_Texture* texturemgr;
	sf::RenderWindow* window;
	sf::Vector2i tiledimension;
	sf::Vector2i mapdimension{ 0,0 };
	unsigned int maxlayers;

	void ReadConfiguration(const ConfigurationType& type, const int& layernum, const std::string& configurationblock);
	bool ReadStaticTiles(const std::string& tilefile);
	void LoadMap(const std::string& mapfile);
	std::string FixConfigurationRow(const ConfigurationType& type, std::string& configurationrow);
public:
	Map(Manager_Texture* mgr, sf::RenderWindow* winptr);
	void Draw();
	
};
#endif
 