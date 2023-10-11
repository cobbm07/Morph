#pragma once
#include <string>
#include "ECS/ECS.h"
/*
* Creates a given map using tiles from the tex_ID created in the asset manager for the tilemap, as well as a scaling factor and tile size
* The scaling factor is used to increase the sprite and transform components to represent textures at a higher resolution, typically a jump from a 32x32 pixel representation to 64x64
* load_map takes in the 2 dimensional integer array in the tile map .csv file, and finds the location of the tile in the tilemap based on the number
*   With a tilemap that is 10 tiles wide, the x location of the target tile is tile_id % 10 multiplied by the tile size, and the y location is tile_id / 10 (integer rounding) multiplied by the tile size
* Then, with the location and size of the tile on the tile map, a tile component is created for each integer in the 2d array
* A second 2d array is then parsed similarly, but this time for the colliders
* If there is a collider on a tile, it will have a number ranging 1-3 for what tile type it is
* A collider component is then created for each integer 1-3 in the array with a tag for what tile it is: ground, water, or lava
*/
class Map {
private:
	std::string tex_ID;
	int map_scale, tile_size, scaled_size;
public:
	Map(std::string t_ID, int m_scale, int t_size);
	void load_map(std::string path, int size_x, int size_y, Manager& m);
};