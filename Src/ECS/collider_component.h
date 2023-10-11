#pragma once
#include <string>
#include "components_include.h"

/*
* The collider component is attached to any entity that should collide with the player (Enemies, terrain, goals)
* Each entity with a collider component has a rectangle outlining the "hitbox", a tag for what collider type it is, source and destination rectangles and a texture to display the colliders
*   The rectangles and texture, as well as the draw method, are only in place for debugging purposes. It is good to verify that the colliders are in the correct place on the screen.
* The collider component utilizes the transform component to create the hitbox area
* Multiple constructors are present for different use cases, as well as for attempting to create different sized/shaped hitboxes to fix issues with collision
* At the bottom of the file is a struct for Collision detection
*   Axix Aligned Bounding Box collision detection is utilized, which is ideal for working with rectangles like I am
*/

struct Collider_Component : public Component {
	SDL_Rect collider;
	std::string tag;

	SDL_Texture* tex;
	SDL_Rect src_r, dest_r;

	Transform_Component* transform;

	Collider_Component(std::string t) {
		tag = t;
	}

	Collider_Component(std::string t, int x_pos, int y_pos, int w, int h) {
		tag = t;
		collider = { x_pos, y_pos, w, h };
	}

	void init() override {
		if (!entity->has_component<Transform_Component>()) {
			entity->add_component<Transform_Component>();
		}
		transform = &entity->get_component<Transform_Component>();

		tex = Game::assets->get_texture("collider");
		src_r = {0, 0, TEX_SIZE, TEX_SIZE};
		dest_r = { collider.x, collider.y, collider.w, collider.h};
	}

	void update() override {
		if (tag != "ground" && tag != "water" && tag != "lava") {
			collider.x = static_cast<int>(transform->position.x + 32);
			collider.y = static_cast<int>(transform->position.y + 8);
			collider.w = transform->scale * (transform->width - 20);
			collider.h = transform->scale * (transform->height - 4);
		}

		dest_r.x = collider.x - Game::camera.x;
		dest_r.y = collider.y - Game::camera.y - 4;
	}

	void draw() override {
		//Asset_Manager::draw(tex, src_r, dest_r, SDL_FLIP_NONE);
	}
};

struct Collision {
	static bool AABB(const SDL_Rect& recA, const SDL_Rect& recB) {
		if (recA.x + recA.w >= recB.x &&
			recB.x + recB.w >= recA.x &&
			recA.y + recA.h >= recB.y &&
			recB.y + recB.h >= recA.y) {
			return true;
		}
		return false;
	}
	static bool AABB(const Collider_Component& colA, const Collider_Component colB) {
		if (AABB(colA.collider, colB.collider)) {
			return true;
		}
		return false;
	}
	static Vector2D AABB_Vec(const Collider_Component& colA, const Collider_Component colB) {
		if (!AABB(colA, colB)) return Vector2D(0,0);
		float axbx = ((colA.collider.x + colA.collider.w) - colB.collider.x); // Player intersects the right side of object
		float ayby = (colA.collider.y + colA.collider.h) - colB.collider.y; // Player intersects the top of the object
		float bxax = (colB.collider.x + colB.collider.w) - colA.collider.x; // Player intersects the left side of object
		float byay = (colB.collider.y + colB.collider.h) - colA.collider.y; // Player intersects the bottom of the object
		float min_x = (axbx < bxax) ? axbx : bxax, min_y = (ayby < byay) ? ayby : byay;
		if (min_x < min_y) {
			return (min_x == axbx) ? Vector2D(axbx, 0) : Vector2D(-bxax, 0);
		}
		return (min_y == ayby) ? Vector2D(0, ayby) : Vector2D(0, -byay);
	}
};