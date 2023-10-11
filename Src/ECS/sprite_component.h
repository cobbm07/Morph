#pragma once
#include "components_include.h"
#include "../game.h"
#include "../asset_manager.h"

/*
* For (Most) of the display and textures, sprite components are used
* If something is visible on the screen, it must have a texture, and to set the texture a sprite component is attached to the entity
* Uses the asset manager to retrieve and set stored textures using only the texture's given name
* Animations are also handled within the sprite component, using just a simple struct to create the animation object with information on how to play the animation
*/

struct Animation {
	int index;
	int frames;
	int speed;

	Animation() {}
	Animation(int i, int f, int s) {
		index = i;
		frames = f;
		speed = s;
	}
};

class Sprite_Component : public Component {
private:
	Transform_Component* transform;
	SDL_Texture* texture;
	SDL_Rect src_rect, dest_rect;

	std::string tex_id;
	bool animated = false;
	int frames = 0;
	int speed = 100;

public:
	int anim_index = 0;
	std::map<const char*, Animation> animations;
	SDL_RendererFlip sprite_flip = SDL_FLIP_NONE;

	Sprite_Component() = default;
	Sprite_Component(std::string id) {
		set_tex(id);
		tex_id = id;
	}

	Sprite_Component(std::string id, bool is_animated) {
		animated = is_animated;
		tex_id = id;

		Animation idle = Animation(0, 2, 300);
		Animation walk = Animation(1, 4, 150);

		animations.emplace("Idle", idle);
		animations.emplace("Walk", walk);

		play("Idle");
		set_tex(id);
	}

	void set_tex(std::string id) {
		texture = Game::assets->get_texture(id);
		tex_id = id;
	}
	std::string get_tex() {
		return tex_id;
	}
	void init() override {
		transform = &entity->get_component<Transform_Component>();

		src_rect.x = src_rect.y = 0;
		src_rect.w = transform->width;
		src_rect.h = transform->height;
	}

	void update() override{
		if (animated) {
			src_rect.x = src_rect.w * static_cast<int>((SDL_GetTicks() / speed) % frames);
			src_rect.y = anim_index * transform->height;
		}
		dest_rect.x = static_cast<int>(transform->position.x) - Game::camera.x;
		dest_rect.y = static_cast<int>(transform->position.y) - Game::camera.y;
		dest_rect.w = transform->width * transform->scale;
		dest_rect.h = transform->height * transform->scale;
	}

	void draw() override {
		Asset_Manager::draw(texture, src_rect, dest_rect, sprite_flip);
	}

	void play(const char* anim_name) {
		frames = animations[anim_name].frames;
		anim_index = animations[anim_name].index;
		speed = animations[anim_name].speed;
	}
};