#ifndef GAMEMAP_H
#define GAMEMAP_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "StreamAttributes.h"
struct StandardTile {
	sf::Sprite tile;
	std::string tilename;
	StandardTile(sf::Texture* texture, const sf::IntRect& texturerect) {

	}
	sf::Vector2f friction;
	friend Attributes* operator>>(Attributes* stream, StandardTile& tile) {

	}
};

using TileID = char;
struct MapTile {
	StandardTile* tiletexture;
	sf::Vector2f position;
};
using StandardTilePtr = std::unique_ptr<StandardTile>;
using TileTextures = std::unordered_map<char, StandardTilePtr>;
using MapTiles = std::vector<std::vector<MapTile>>;

class Map {
protected:
	sf::Vector2f tiledimension;
	MapTiles maptiles;
	TileTextures tiletextures;
	std::string name;
	float gravity;
	sf::Texture* background;
	sf::RenderWindow* winptr;
	void ReadStandardTiles(const std::string& tilefile);
public:
	Map(sf::RenderWindow* winptr);
	void LoadMap(const std::string& mapfile);
	MapTile* GetTile(const float& x, const float& y, const unsigned int& elevation) const;
	sf::Vector2f GetTileSize() const;
	void Update(const float& dT);
	void Draw();


};




#endif