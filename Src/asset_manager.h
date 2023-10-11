#pragma once
#include <map>
#include <string>
#include "game.h"
#include "vector2D.h"
#include "ECS/ECS.h"

/*
* Manages assets, which currently just means textures
* Allows addition of textures to a map so the user can get the texture using the "key" name
*/

class Asset_Manager {
private:
	Manager* manager;
	std::map<std::string, SDL_Texture*> textures;
public:
	Asset_Manager(Manager* man);

	static SDL_Texture* load_texture(const char* path);
	static void draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip);

	void add_texture(std::string id, const char* path);
	SDL_Texture* get_texture(std::string id);
};