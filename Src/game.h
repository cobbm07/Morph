#pragma once
#include <SDL.h>
#include <iostream>
#include <vector>

/*
* The Game class is where most of the work happens
* Many constant values are declared to lock values in that should not change any other way than manually
*   This prevents any user mistakes by setting incorrect values for hte map size, for example
*   This also allows one variable to control many other values. Instead of declaring a texture size of 32 each time a texture size is needed, I can declare it once here
*   Later, if I want to implement higher resolution textures, I need only to change one value
* Also of note are methods for updating the game variables each frame, rendering/redrawing sprites each frame, event handling (which really just consists of clicking the x button to close the game), and the clean() method which frees up memory before closing
*/

static const int WINDOW_W = 1000, WINDOW_H = 600, TEX_SIZE = 32, PLAYER_SCALE = 3, TILE_SCALE = 2, MAP_W = 32, MAP_H = 32, TILE_SCALED_SIZE = TEX_SIZE * TILE_SCALE;

struct Collider_Component;
class Asset_Manager;

class Game {
private:
	SDL_Window* window;
	int count = 0;
public:
	static bool is_running, in_air, won, lost;

	void init(const char* title, int x_pos, int y_pos, int width, int height, bool fullscreen);

	void handle_events();
	void update();
	void render();
	void clean();

	void load_level(int lvl_id);

	bool running();

	static SDL_Renderer* renderer;
	static SDL_Event event;
	static SDL_Rect camera;
	static Asset_Manager* assets;
	enum group_labels : std::size_t {
		group_map,
		group_players,
		group_colliders,
		group_goal,
		group_enemies,
		group_ui,
	};
};