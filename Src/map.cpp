#include "map.h"
#include "game.h"
#include <fstream>
#include <sstream>
#include "ECS/components_include.h"

Map::Map(std::string t_ID, int m_scale, int t_size) {
	tex_ID = t_ID;
	map_scale = m_scale;
	tile_size = t_size;
	scaled_size = m_scale * t_size;
}

void Map::load_map(std::string path, int size_x, int size_y, Manager& man) {
	std::ifstream map_file;
	map_file.open(path);
	std::string line;

	for (int y = 0; y < size_y; y++) {
		std::getline(map_file, line);
		std::stringstream line_stream(line);
		std::string substr;
		for (int x = 0; x < size_x; x++) {
			std::getline(line_stream, substr, ',');
			int tile_id = std::stoi(substr);
			int x_loc = (tile_id % 10) * tile_size, y_loc = (tile_id / 10) * tile_size;

			auto& tile(man.add_entity());
			tile.add_component<Tile_Component>(x_loc, y_loc, x * scaled_size, y * scaled_size, tile_size, map_scale, tex_ID);
			tile.add_group(Game::group_map);
		}
	}

	map_file.ignore();

	for (int y = 0; y < size_y; y++) {
		std::getline(map_file, line);
		std::stringstream line_stream(line);
		std::string substr;
		for (int x = 0; x < size_x; x++) {
			std::getline(line_stream, substr, ',');
			int col_id = std::stoi(substr);

			if (col_id > 0) {
				auto& t_col(man.add_entity());
				t_col.add_group(Game::group_colliders);

				std::string tag;
				switch (col_id) {
				case 1:
					tag = "ground";
					break;
				case 2:
					tag = "water";
					break;
				case 3:
					tag = "lava";
					break;
				default:
					tag = "";
					break;
				}
				t_col.add_component<Collider_Component>(tag, x * scaled_size, y * scaled_size, scaled_size, scaled_size);
			}
		}
	}
	map_file.close();
}