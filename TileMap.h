#ifndef TILEMAP_H
#define TILEMAP_H
#include <memory>
#include <string>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <vector>
#include <unordered_map>

namespace sf {
	using Vector2ush = sf::Vector2<unsigned short>;
	using Vector2C = sf::Vector2<char>;
	class RenderTarget;
}

namespace MapData {
	static const unsigned int TILE_PIXEL_DIMENSION{ 32 };
	static sf::Vector2f MapToWorldCoords(const sf::Vector2u& map_coords) {
		return sf::Vector2f{ static_cast<float>(map_coords.x * TILE_PIXEL_DIMENSION), static_cast<float>(map_coords.y * TILE_PIXEL_DIMENSION) };
	}
	static sf::Vector2u WorldToMapCoords(const sf::Vector2f& world_coords) {
		sf::Vector2u res{ static_cast<unsigned int>(world_coords.x / TILE_PIXEL_DIMENSION), static_cast<unsigned int>(world_coords.y / TILE_PIXEL_DIMENSION) };
		return res; //RVO

	}
	using TileID = char;
	//***Flyweight pattern***
	struct MasterTile {
		sf::Sprite tile_sprite;
		bool solid{ true };
		MasterTile() {

		}
	};
	struct WorkerTile {
		TileID master_id;
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
	using WorkerTiles = std::vector<std::vector<WorkerTile>>;
	using WorkerTiles2 = std::vector<WorkerTiles>;
	
	struct MapImpl {//Impl for map.

		sf::Vector2u map_tile_dimensions;
		sf::Vector2f tile_pixel_dimensions;
		std::shared_ptr<sf::Texture> tile_sheet;
		WorkerTiles2 worker_tiles;
		MasterTiles master_tiles;
		MapImpl(sf::Vector2u&& m_t_d, sf::Vector2f&& t_p_d, std::shared_ptr<sf::Texture>&& t_s)
			:map_tile_dimensions(std::move(m_t_d)),
			tile_pixel_dimensions(std::move(t_p_d)),
			tile_sheet(std::move(t_s)),
			worker_tiles(WorkerTiles2(map_tile_dimensions.y*map_tile_dimensions.x))
			//worker_tiles(WorkerTiles(map_tile_dimensions.y, std::vector<WorkerTile>(map_tile_dimensions.x)))
		{

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