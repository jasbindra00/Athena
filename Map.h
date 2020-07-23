#ifndef GAMEMAP_H
#define GAMEMAP_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
struct TileTexture {
	sf::Sprite tile;
	sf::IntRect tilerect;
	TileTexture(sf::Texture* texture, const sf::IntRect& texturerect);
	float friction;
};

struct MapTile {
	TileTexture* tiletexture;
	sf::Vector2f position;
};
using TileTexture = std::unique_ptr<TileTexture>;
using TileTextures = std::unordered_map<std::string, TileTexture>;
using MapTiles = std::vector<std::vector<MapTile>>;

class Map {
protected:
	MapTiles maptiles;
	TileTextures tiletextures;

	float gravity;
	sf::Texture* background;
	sf::RenderWindow* winptr;
public:
	Map(sf::RenderWindow* winptr);
	void LoadMap(const std::string& mapfile);
	MapTile* GetTile(const float& x, const float& y, const unsigned int& elevation) const;
	sf::Vector2f GetTileSize() const;
	void Update(const float& dT);
	void Draw();


};




#endif