#include "game.h"
#include "map.h"
#include "ECS/components_include.h"
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <algorithm>
#include <sstream>
#include <fstream>

//Global Variables
Map* map;
Manager manager;
SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
SDL_Rect Game::camera = {0, 40, (MAP_W * TILE_SCALED_SIZE) - WINDOW_W, (MAP_H * TILE_SCALED_SIZE) - WINDOW_H};
Asset_Manager* Game::assets = new Asset_Manager(&manager);
bool Game::is_running = false;

bool Game::in_air = false;
bool Game::won = false;
bool Game::lost = false;

int player_lives = 3;
int frames_since_dmg = 0;
int goals_obtained = 0;
int current_level = 1;

//Add Entities
auto& player(manager.add_entity());

std::vector<Entity*> enemies;
std::vector<Entity*> goals;

Entity* health_bar[3]{&manager.add_entity(), &manager.add_entity(), &manager.add_entity()};

//Initialize window, renderer, game assets
void Game::init(const char* title, int x_pos, int y_pos, int width, int height, bool fullscreen) {
	PlaySound(TEXT("assets\\sounds\\morph_music.wav"), NULL, SND_LOOP | SND_ASYNC);
	int flags = 0;
	if (fullscreen)
		flags = SDL_WINDOW_FULLSCREEN;
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		window = SDL_CreateWindow(title, x_pos, y_pos, width, height, flags);
		if (window) {
			renderer = SDL_CreateRenderer(window, -1, 0);
			if (renderer) {
				SDL_SetRenderDrawColor(renderer, 173, 255, 243, 255);
				is_running = true;
			}
		}
	}

	//Textures
	assets->add_texture("terrain", "assets/images/tilemap.png");

	assets->add_texture("base_char", "assets/images/base_char_anims.png");
	assets->add_texture("water_char", "assets/images/water_char_anims.png");
	assets->add_texture("lava_char", "assets/images/lava_char_anims.png");

	assets->add_texture("lava_enemy", "assets/images/lava_enemy_anims.png");
	assets->add_texture("water_enemy", "assets/images/water_enemy_anims.png");

	assets->add_texture("heart_empty", "assets/images/empty_heart.png");
	assets->add_texture("heart_full", "assets/images/full_heart.png");

	assets->add_texture("collider", "assets/images/collider_tex.png");
	assets->add_texture("goal", "assets/images/goal.png");

	//Map

	player.add_component<Transform_Component>(100, 1760, TEX_SIZE, TEX_SIZE, PLAYER_SCALE);
	player.add_component<Sprite_Component>("base_char", true);
	player.add_component<Keyboard_Controller>();
	player.add_component<Collider_Component>("player", 100 + (12 * PLAYER_SCALE), 1760, 12 * PLAYER_SCALE, TEX_SIZE * PLAYER_SCALE);
	player.add_group(group_players);

	load_level(current_level);

	//Health Bar
	for (int i = 0; i < player_lives; i++) {
		health_bar[i]->add_component<Transform_Component>(20 + (TEX_SIZE * i), 1460, TEX_SIZE, TEX_SIZE, TILE_SCALE);
		health_bar[i]->add_component<Sprite_Component>("heart_full");
		health_bar[i]->add_group(group_ui);
	}
}

//Set Groups
auto& tiles(manager.get_group(Game::group_map));
auto& colliders(manager.get_group(Game::group_colliders));
auto& user_interface(manager.get_group(Game::group_ui));

void Game::load_level(int lvl_id) {
	tiles.clear();
	enemies.clear();
	goals.clear();
	colliders.clear();
	in_air = false;
	map = new Map("terrain", TILE_SCALE, TEX_SIZE);
	map->load_map("assets/documents/map_" + std::to_string(lvl_id) + ".csv", MAP_W, MAP_H, manager);

	//Get player count + locations, enemy count + locations, goal count + locations
	std::ifstream level_file;
	level_file.open("assets/documents/lvl_" + std::to_string(lvl_id) + "_info.csv");
	std::string line;
	std::getline(level_file, line);
	std::getline(level_file, line);

	std::stringstream line_stream(line);
	std::string substr;
	std::getline(line_stream, substr, ',');
	int player_x = std::stoi(substr);
	std::getline(line_stream, substr, ',');
	int player_y = std::stoi(substr);

	player.get_component<Transform_Component>().position = Vector2D(player_x, player_y);

	std::getline(level_file, line);
	line_stream.str("");
	line_stream.clear();
	line_stream << line;

	std::getline(line_stream, substr, ',');
	std::getline(line_stream, substr, ',');
	int count = std::stoi(substr);
	for (int i = 0; i < count; i++) { //Add Enemies
		std::getline(level_file, line);
		line_stream.str("");
		line_stream.clear();
		line_stream << line;
		std::getline(line_stream, substr, ',');
		int enemy_x = std::stoi(substr);
		std::getline(line_stream, substr, ',');
		int enemy_y = std::stoi(substr);
		std::getline(line_stream, substr, ',');
		std::string enemy_id = substr;

		enemies.push_back(&manager.add_entity());
		enemies.at(i)->add_component<Transform_Component>(enemy_x, enemy_y, TEX_SIZE, TEX_SIZE, PLAYER_SCALE);
		enemies.at(i)->add_component<Sprite_Component>(enemy_id, true);
		enemies.at(i)->add_component<Collider_Component>(enemy_id);
		enemies.at(i)->add_group(group_enemies);
	}

	std::getline(level_file, line);
	line_stream.str("");
	line_stream.clear();
	line_stream << line;

	std::getline(line_stream, substr, ',');
	std::getline(line_stream, substr, ',');
	count = std::stoi(substr);
	for (int i = 0; i < count; i++) { //Add Goals
		std::getline(level_file, line);
		line_stream.str("");
		line_stream.clear();
		line_stream << line;
		std::getline(line_stream, substr, ',');
		int goal_x = std::stoi(substr);
		std::getline(line_stream, substr, ',');
		int goal_y = std::stoi(substr);

		goals.push_back(&manager.add_entity());
		goals.at(i)->add_component<Transform_Component>(goal_x, goal_y, TEX_SIZE, TEX_SIZE, TILE_SCALE);
		goals.at(i)->add_component<Sprite_Component>("goal", false);
		goals.at(i)->add_component<Collider_Component>("goal");
		goals.at(i)->add_group(group_goal);
	}
}

//Esc to quit
void Game::handle_events() {
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	}
}

//Update variables every frame
void Game::update() {
	if (frames_since_dmg > 0) frames_since_dmg--;
	bool take_dmg = false;

	SDL_Rect player_col = player.get_component<Collider_Component>().collider;
	Vector2D player_pos = player.get_component<Transform_Component>().position;
	Transform_Component* player_transform = &(player.get_component<Transform_Component>());

	manager.refresh();
	manager.update();

	//Check Player - Enemy Collision
	if (frames_since_dmg == 0) { // Do not run if player damage is on cooldown; waste of time
		enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&take_dmg](Entity* e) {
			if (Collision::AABB(e->get_component<Collider_Component>(), player.get_component<Collider_Component>())) {
				if (e->get_component<Collider_Component>().tag == "water_enemy") {
					if (player.get_component<Sprite_Component>().get_tex() == "water_char") {
						std::cout << "Player eliminated Water Enemy!" << std::endl;
						return true;
					}
					else {
						std::cout << "Water Enemy hurt Player!" << std::endl;
						take_dmg = true;
						return false;
					}
				}
				if (e->get_component<Collider_Component>().tag == "lava_enemy") {
					if (player.get_component<Sprite_Component>().get_tex() == "lava_char") {
						std::cout << "Player eliminated Lava Enemy!" << std::endl;
						return true;
					}
					else {
						std::cout << "Lava Enemy hurt Player!" << std::endl;
						take_dmg = true;
						return false;
					}
				}
			}
			return false;
		}), enemies.end());
	}

	//Check Player - Terrain Collision
	std::for_each(colliders.begin(), colliders.end(), [&take_dmg](Entity* c) {
		if (Collision::AABB(c->get_component<Collider_Component>(), player.get_component<Collider_Component>())) {
			Vector2D shift_vec = Collision::AABB_Vec(c->get_component<Collider_Component>(), player.get_component<Collider_Component>());

			// Damage
			if (frames_since_dmg == 0) {
				if (c->get_component<Collider_Component>().tag == "water" && player.get_component<Sprite_Component>().get_tex() != "water_char") {
					std::cout << "Player hit water!" << std::endl;
					take_dmg = true;
				}
				else if (c->get_component<Collider_Component>().tag == "lava" && player.get_component<Sprite_Component>().get_tex() != "lava_char") {
					std::cout << "Player hit lava!" << std::endl;
					take_dmg = true;
				}
			}

			// Special Cases
			if (player.get_component<Transform_Component>().velocity.y != 0)
				in_air = true;
			if (shift_vec.y <= 0)
				in_air = false;
			if (shift_vec.y != 0)
				player.get_component<Transform_Component>().velocity.y = 0;
			if (player.get_component<Transform_Component>().velocity.y != 0)
				in_air = true;
			player.get_component<Transform_Component>().position += shift_vec;
		}
	});

	//Check Player - Goal Collision
	goals.erase(std::remove_if(goals.begin(), goals.end(), [](Entity* g) {
		if (Collision::AABB(g->get_component<Collider_Component>(), player.get_component<Collider_Component>())) {
			return true;
			goals_obtained++;
		}
		return false;
	}), goals.end());

	if (goals.empty()) {
		current_level++;
		if (current_level < 3)
			Game::load_level(current_level);
		else {
			won = true;
			is_running = false;
		}
	}
	if (in_air)
		player_transform->velocity.y += 0.1f;

	if (take_dmg) {
		if (frames_since_dmg == 0) {
			health_bar[player_lives - 1]->get_component<Sprite_Component>().set_tex("heart_empty");
			player_lives--;
			frames_since_dmg = 90;
		}
	}
	if (player_lives == 0) {
		lost = true;
		is_running = false;
	}

	//Move camera
	camera.x = player_transform->position.x -(WINDOW_W / 2) + ((TILE_SCALED_SIZE * 3) / 4);
	camera.y = player_transform->position.y -(WINDOW_H / 2);

	//Don't show area outside the map
	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > camera.w)
		camera.x = camera.w;
	if (camera.y > camera.h)
		camera.y = camera.h;

	//Keep health bar at the same location
	for (int i = 0; i < 3; i++)
		health_bar[i]->get_component<Transform_Component>().position = Vector2D(camera.x + (i * TILE_SCALED_SIZE), camera.y);
}

//Redraw assets every frame
void Game::render() {
	SDL_RenderClear(renderer);

	std::for_each(tiles.begin(), tiles.end(), [](Entity* e) {e->draw(); });
	player.draw();
	std::for_each(enemies.begin(), enemies.end(), [](Entity* e) {e->draw(); });
	std::for_each(goals.begin(), goals.end(), [](Entity* e) {e->draw(); });
	std::for_each(user_interface.begin(), user_interface.end(), [](Entity* e) {e->draw(); });

	SDL_RenderPresent(renderer);
}

//Clean assets before closing
void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}

bool Game::running() { return is_running; }