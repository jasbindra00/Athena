#ifndef TILEMAP_H
#define TILEMAP_H
#include <memory>
#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <unordered_map>
#include "Manager_Texture.h"
namespace sf {
	using Vector2ush = sf::Vector2<unsigned short>;
	using Vector2C = sf::Vector2<char>;
	class RenderTarget;
}

namespace MapData {
	
	static const unsigned int TILE_PIXEL_DIMENSION{ 32 };
	



	static sf::Vector2f MapToWorldCoords(const sf::Vector2u& map_coords, const sf::Vector2u& tile_pixel_dimensions = { TILE_PIXEL_DIMENSION, TILE_PIXEL_DIMENSION }) {
		return sf::Vector2f{ static_cast<float>(map_coords.x * tile_pixel_dimensions.x), static_cast<float>(map_coords.y * tile_pixel_dimensions.y) };
	}
	static sf::Vector2u WorldToMapCoords(const sf::Vector2f& world_coords, const sf::Vector2u& tile_pixel_dimensions = { TILE_PIXEL_DIMENSION, TILE_PIXEL_DIMENSION }) {
		sf::Vector2u res{ static_cast<unsigned int>(world_coords.x / tile_pixel_dimensions.x), static_cast<unsigned int>(world_coords.y / tile_pixel_dimensions.y) };
		return res; //RVO

	}
	static unsigned int MapToArrayCoords(const sf::Vector2u& map_coords, const sf::Vector2u& map_tile_dimensions) {
		return map_coords.y * map_tile_dimensions.x + map_coords.x;
	}
	static sf::Vector2u LimitMapCoords (sf::Vector2u map_coords, const sf::Vector2u& map_tile_dimensions)
	{
		if (map_coords.x < 0) map_coords.x = 0;
		else if (map_coords.x >= map_tile_dimensions.x) map_coords.x = map_tile_dimensions.x;
		if (map_coords.y < 0) map_coords.y = 0;
		else if (map_coords.y >= map_tile_dimensions.y) map_coords.y = map_tile_dimensions.y;
		return map_coords; //RVO
	};
	using TileID = unsigned int;
	//***Flyweight pattern***
	struct MasterTile {
		sf::Sprite tile_sprite;
		bool solid{ true };
		MasterTile(const sf::FloatRect& texture_rect, SharedTexture atlas_map) {
			tile_sprite.setTexture(*atlas_map);
			tile_sprite.setTextureRect(static_cast<sf::IntRect>(texture_rect));
		}
		MasterTile() {

		}
		void InitTile(const sf::FloatRect& texture_rect, SharedTexture atlas_map) {
			tile_sprite.setTexture(*atlas_map);
			tile_sprite.setTextureRect(static_cast<sf::IntRect>(texture_rect));
		}
	};
	struct WorkerTile {
		TileID master_id{ UINT_MAX };
		sf::Vector2u position;
		sf::Vector2f friction{ 0,0 };
		bool teleport{ false };
		bool deadly{ false };
		char layer_number{ '0' };

		void ReadIn() {
			//For configuration read in
		}
	};
	//*** ***

	using MasterTiles = std::unordered_map<TileID, MasterTile>;
	using WorkerTiles = std::vector<WorkerTile>;

	struct MapImpl {//Impl for map.
	private:
		static void CreateDefaultTileTexture(Manager_Texture* texture_mgr) {
			std::string default_tile_texture_name{ "default_tile_texture.png" }; //MAKE STATIC CONST
			//Test if the default texure already exists by trying to load it in.
			{
				sf::Texture default_texture;
				if (default_texture.loadFromFile(default_tile_texture_name)) return;

			}
			//The texture does not exist. We must create it.
			sf::RenderTexture default_tile_texture;
			default_tile_texture.create(32, 32);

			sf::RectangleShape default_tile;
			default_tile.setSize(sf::Vector2f{ 32,32 });
			default_tile.setFillColor(sf::Color::Color(190, 190, 190, 255));
			default_tile.setOutlineColor(sf::Color::Black);
			default_tile.setOutlineThickness(-2);
			default_tile_texture.draw(default_tile);


			sf::Image texture_png = default_tile_texture.getTexture().copyToImage();
			texture_png.saveToFile(default_tile_texture_name);
				
		}
	public:
		sf::Vector2u map_tile_dimensions;
		sf::Vector2u tile_pixel_dimensions;
		sf::Vector2u atlas_tile_dimensions;
		std::shared_ptr<sf::Texture> tile_sheet;
		WorkerTiles worker_tiles;
		MasterTiles master_tiles;
		MapImpl(sf::Vector2u&& m_t_d, sf::Vector2u&& t_p_d, std::shared_ptr<sf::Texture>&& t_s, Manager_Texture* texture_mgr)
			:map_tile_dimensions(std::move(m_t_d)),
			tile_pixel_dimensions(std::move(t_p_d)),
			tile_sheet(std::move(t_s)),
			worker_tiles(WorkerTiles(map_tile_dimensions.y*map_tile_dimensions.x))
		{
			atlas_tile_dimensions = sf::Vector2u{ static_cast<unsigned int>(tile_sheet->getSize().x / tile_pixel_dimensions.x), static_cast<unsigned int>(tile_sheet->getSize().y / tile_pixel_dimensions.y) };
			std::string default_tile_texture_name{ "default_tile_texture.png" };
			//Create the default tile texture if it does not exist.
			CreateDefaultTileTexture(texture_mgr);
			//Register the default tile texture in master_tiles.

			master_tiles.insert({ UINT_MAX, MasterTile{sf::FloatRect{ 0,0,32,32 }, texture_mgr->RequestResource(default_tile_texture_name)} });
		}
		
		bool RegisterTile(const sf::Vector2u& tile_atlas_coordinates) {
			//take the tile co-ordinates, convert them into array_coordinates.
			unsigned int tile_array_position = MapToArrayCoords(tile_atlas_coordinates, tile_pixel_dimensions);
			if (master_tiles.find(tile_array_position) != master_tiles.end()) return false; //The tile has already been registered.
			master_tiles[std::move(tile_array_position)] = MasterTile(sf::FloatRect{ static_cast<float>(tile_atlas_coordinates.x * tile_pixel_dimensions.x), static_cast<float>(tile_atlas_coordinates.y * tile_pixel_dimensions.y), static_cast<float>(tile_pixel_dimensions.x), static_cast<float>(tile_pixel_dimensions.y) }, tile_sheet);
			return true;
		}
		void PlaceTile(const sf::Vector2u& tile_map_coordinates, const sf::Vector2u& tile_atlas_coordinates) {
			//Obtain the tileID from the tile_atlas_coordinates, and obtain the master tile.
			//Obtain the array position from tile_map_coordinates and change the tileid of that tile to the master tile.
			unsigned int arr_pos = MapToArrayCoords(tile_map_coordinates, map_tile_dimensions);
			unsigned int tile_id = MapToArrayCoords(tile_atlas_coordinates, atlas_tile_dimensions); //Find the tile dimensions of the tile_sheet.
			worker_tiles.at(arr_pos).master_id = tile_id;
		}
	};

	using MapImplPtr = std::unique_ptr<MapImpl>; //pImpl
	class TileMap {
	private:
		MapImplPtr map_tiles;
	public:
		TileMap();
		void ReadIn(const std::string& map_file); //Reads the configuration file created by the level editor.
		void Update(const float& dT);
		void Rasterize(sf::RenderTarget& target) const;
		// 		const WorkerTile& GetWorkerTile(const sf::Vector2u& map_position); //Throwing function.
		// 		const MasterTile& GetMasterTile(const TileID& id); //Throwing function.
	};
}

#endif