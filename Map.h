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
namespace MapData {
	using namespace TileData;
	using StaticTilePtr = std::unique_ptr<StaticTile>;
	using StaticTiles = std::unordered_map<char, StaticTilePtr>;
	using MapTiles = std::vector<std::vector<MapTile>>;
	
	namespace ConfigurationData {
		using ConfigurationReader = std::function<bool(std::string, int)>;
		enum class ConfigurationType {
			GAME, LAYER, TELEPORT, DEADLY, NULLTYPE
		};
		EnumConverter<ConfigurationType> configurationconverter([](const std::string& str)->ConfigurationType {
			if (str == "GAME") return ConfigurationType::GAME;
			else if (str == "LAYER") return ConfigurationType::LAYER;
			else if (str == "TELEPORT") return ConfigurationType::TELEPORT;
			else if (str == "DEADLY") return ConfigurationType::DEADLY;
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
	sf::Vector2i tiledimension;
	sf::Vector2i mapdimension{ 0,0 };

	template<typename T, typename  = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T>, ConfigurationType>>>
	void ReadConfiguration(const std::string& configurationblock) {
		ConfigurationReader reader = configurationreaders[T];
		Attributes configurationstream(configurationblock);
		std::string errorstring;
		std::string alteredstring;
		int rownumber{ -1 };
		while (!configurationstream.eof()) {
			++rownumber;
			std::string configurationrow(configurationstream.GetWord());
			{//ensuring that all the rows match the specified map dimension.
				std::string configrow = configurationstream.PeekWord();
				if (configrow.size() != mapdimension.x) {
					std::string altered;
					if (configrow.size() < mapdimension.x) {
						std::string fill(mapdimension.x - configrow.size(), '0');
						configrow.append(std::move(fill));
					}
					else {configrow = configrow.substr(0, mapdimension.x);
						alteredstring.push_back(std::to_string(rownumber) + " "); //log.
					}
				}
				try { reader(std::move(configurationrow), rownumber); }
				catch (const CustomException& exception) {
					auto rowerror = static_cast<std::string>(exception.what());
					errorstring.append(rowerror);
				}
			}
		}
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Map CONFIGURATION ERROR : The following configuration indices were truncated/extended due to a mismatch between the size and the specified map X dimension : " + alteredstring);
		LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, std::move(errorstring));
	}
	void ReadConfiguration(const std::string& configurationtype, const std::string& configuration);
	bool ReadStandardTiles(const std::string& tilefile);
	void LoadMap(const std::string& mapfile);


public:
	Map(Manager_Texture* mgr); 

};
#endif
 