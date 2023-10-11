#pragma once
#include "components_include.h"

/*
* The keyboard controller is a component that should only be attached one object: the player
* Attaching this component allows the user to control the entity, with behaviors defined in the update() function
* The update() function checks each frame whether user input has been recieved,and if so decides how to deal with it using switch statements based on the key pressed
* From a programmer's position, it is very easy to set up behavior for a key by simply setting up a switch case for the key, and writing exactly what the key press/release should do
*/

struct Keyboard_Controller : public Component {
	Transform_Component* transform;
	Sprite_Component* sprite;

	void init() override {
		transform = &entity->get_component<Transform_Component>();
		sprite = &entity->get_component<Sprite_Component>();
	}

	void update() override {
		if (Game::event.type == SDL_KEYDOWN) {
			switch (Game::event.key.keysym.sym) {
			case SDLK_w: case SDLK_UP:
				if (!Game::in_air) {
					transform->velocity.y = -3.2 ;
					Game::in_air = true;
				}
				break;
			case SDLK_a: case SDLK_LEFT:
				transform->velocity.x = -1.35;
				sprite->play("Walk");
				sprite->sprite_flip = SDL_FLIP_HORIZONTAL;
				break;
			case SDLK_d: case SDLK_RIGHT:
				transform->velocity.x = 1.35;
				sprite->play("Walk");
				break;
			case SDLK_1:
				sprite->set_tex("base_char");
				break;
			case SDLK_2:
				sprite->set_tex("water_char");
				break;
			case SDLK_3:
				sprite->set_tex("lava_char");
				break;
			}
		}

		if (Game::event.type == SDL_KEYUP) {
			switch (Game::event.key.keysym.sym) {
			case SDLK_a: case SDLK_LEFT:
				transform->velocity.x = 0;
				sprite->play("Idle");
				sprite->sprite_flip = SDL_FLIP_NONE;
				break;
			case SDLK_d: case SDLK_RIGHT:
				transform->velocity.x = 0;
				sprite->play("Idle");
				break;
			case SDLK_ESCAPE:
				Game::is_running = false;
			}
		}
	}
};