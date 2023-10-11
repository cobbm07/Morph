#pragma once
#include "../vector2D.h"
#include "../game.h"
/*
* Each "Tile" of the map has a Tile component
* The tiles update their position based on the camera position, so they...
*   1) Do not move when the player is outside the range of the map, preventing the user from seeing things outside the bounds of the map
*   2) Move when the character moves, so even when the character is in the center of the screen, they can still traverse the map
*/
class Tile_Component : public Component{
public:
	SDL_Texture* texture;
	SDL_Rect src_rect, dest_rect;
	Vector2D position;

	Tile_Component() {}
	Tile_Component(int src_x, int src_y, int x_pos, int y_pos, int t_size, int t_scale, std::string id) {
		texture = Game::assets->get_texture(id);

		position.x = x_pos;
		position.y = y_pos;

		src_rect = { src_x, src_y, t_size, t_size };
		dest_rect.w = dest_rect.h = t_size * t_scale;
	}

	~Tile_Component() {
		SDL_DestroyTexture(texture);
	}

	void update() override {
		dest_rect.x = position.x - Game::camera.x;
		dest_rect.y = position.y - Game::camera.y;
	}

	void draw() override {
		Asset_Manager::draw(texture, src_rect, dest_rect, SDL_FLIP_NONE);
	}
};