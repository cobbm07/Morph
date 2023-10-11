#pragma once
#include "../vector2D.h"

/*
* A Transform component allows for easy modification of the location of the entity using it
* The entity now has a position and velocity, and can move about the screen according to it
*/

struct Transform_Component : public Component {
	Vector2D position;
	Vector2D velocity;

	int speed = 3;
	int scale = 1;
	int width = TEX_SIZE;
	int height = TEX_SIZE;

	Transform_Component() {}
	Transform_Component(float x, float y, float w, float h, int sc) {
		position.x = x;
		position.y = y;
		width = w;
		height = h;
		scale = sc;
	}

	void update() override {
		position.x += velocity.x * speed;
		position.y += velocity.y * speed;
	}
};