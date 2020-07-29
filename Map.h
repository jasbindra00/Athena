#ifndef GAMEMAP_H
#define GAMEMAP_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include "StreamAttributes.h"
#include "Manager_Texture.h"
#include "KeyProcessing.h"


namespace ConfigurationData {
	enum class ConfigurationType {
		GAME_MAP = 0, LAYER_MAP, WARP_MAP, DEADLY_MAP, NULLTYPE
	};
	EnumConverter<ConfigurationType> converter([](const std::string& type) {
		if (type == "GAME_MAP") return ConfigurationType::GAME_MAP;
		else if (type == "LAYER_MAP") return ConfigurationType::LAYER_MAP;
		else if (type == "WARP_MAP") return ConfigurationType::WARP_MAP;
		else if (type == "DEADLY_MAP") return ConfigurationType::DEADLY_MAP;
		return ConfigurationType::NULLTYPE;
		});

}
class StandardTile {

public:
	std::string atlasmapname;
	SharedTexture texture;
	sf::Sprite tilesprite;
	sf::IntRect texturerect;
	sf::Vector2f friction;
	StandardTile() {
	}
};
using TileID = char;
struct MapTile {
	StandardTile* standardtile;
	sf::Vector2f position;
	unsigned int layer;
};
using StandardTilePtr = std::unique_ptr<StandardTile>;
using StandardTiles = std::unordered_map<char, StandardTilePtr>;
using MapTiles = std::vector<std::vector<MapTile>>;
class Map;
using ConfigurationReader = std::function<void(Map*, Attributes*)>;
using ConfigurationReaders = std::unordered_map<ConfigurationData::ConfigurationType, ConfigurationReader>;

class Map {
private:
	static void CreateConfigurationMap() {
		ConfigurationReaders configreaders;
		configreaders[ConfigurationData::ConfigurationType::GAME_MAP] = [](Map* map, Attributes* stream) {

		};
	}
protected:
	static ConfigurationReaders configurationreaders;

	MapTiles maptiles;
	StandardTiles standardtiles;
	std::string tilefile;
	sf::Vector2i tiledimension;
	sf::Vector2i mapdimension;
	unsigned int maxlayers{ 0 };
	std::string mapname;
	float gravity;
	sf::Texture* background;
	sf::RenderWindow* renderwindow;
	Manager_Texture* texturemgr;
	std::string CreateZeroConfig() {

	}
	MapTile* GetMapTile(const unsigned int& ind1, const unsigned int& ind2);
	static void ReadMapAttributes();
	bool ReadStandardTiles(const std::string& tilefile);
	void LoadMap(const std::string& mapfile);
	void ReadConfiguration(const std::string& configurationtype, const std::string& configuration);
	StandardTile* GetErrorTile() {
		return nullptr;
	}
public:
	Map(Manager_Texture* mgr, sf::RenderWindow* winptr);
	MapTile* GetTile(const float& x, const float& y, const unsigned int& elevation) const;
	sf::Vector2i GetTileSize() const;
	void Update(const float& dT);
	void Draw();
};


ConfigurationReaders readers;
readers[ConfigurationData::CONFIGTYPE::GAME_MAP] = [](Attributes* stream, Map* map) {

}
ConfigurationReaders Map::configurationreaders[ConfigurationData::CONFIGTYPE::GAME_MAP] = [](Attributes* stream, Map* map) {
	auto configurationrow = configurationstream.GetWord();
	maptiles.push_back(std::vector<MapTile>{});
	for (int i = 0; i < configurationrow.size(); ++i) {
		MapTile tile;
		auto standardtileexists = standardtiles.find(configurationrow[i]);
		if (standardtileexists == standardtiles.end()) {
			tile.standardtile = GetErrorTile();
			LOG::Log(LOCATION::MAP, LOGTYPE::ERROR, __FUNCTION__, "Unable to find tile of TILEID char of " + std::to_string(configurationrow[i]) + "at index " + ConvertToIndexNotation(configrow, i));
		}
		else tile.standardtile = standardtileexists->second.get();
		tile.position = sf::Vector2f{ static_cast<float>(i * tiledimension.x), static_cast<float>(configrow * tiledimension.y) };
		maptiles.back().push_back(std::move(tile));
	}
}



#endif