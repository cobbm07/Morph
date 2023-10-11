#include "asset_manager.h"
#include "ECS/components_include.h"
#include <SDL_image.h>

Asset_Manager::Asset_Manager(Manager* man) {
	manager = man;
}

void Asset_Manager::add_texture(std::string id, const char* path) {
	textures.emplace(id, Asset_Manager::load_texture(path));
}

SDL_Texture* Asset_Manager::get_texture(std::string id) {
	return textures[id];
}

SDL_Texture* Asset_Manager::load_texture(const char* path) {
	return SDL_CreateTextureFromSurface(Game::renderer, IMG_Load(path));
}

void Asset_Manager::draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip) {
	SDL_RenderCopyEx(Game::renderer, tex, &src, &dest, NULL, NULL, flip);
}