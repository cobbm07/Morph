#pragma once
#include <iostream>
#include <vector>
#include <bitset>
#include <array>

/*
* The Entity Component System is complicated, but the basics of it is this:
* Each object you put in the game is an entity, managed by a single Manager, and each entity is made up of components
* Entities:
*   An entity can have at most max_components components, but as few as 0 (similar process used for groups)
*   An entity is constructed using a manager (see below), and each frame its components are updated via update() and drawn with draw()
*   Using template types and ellipsis in the arguments, all component types can be added with the same add_component function regardless of what component type it is and how many initializer values are needed
*     The only thing to be aware of is that it can be trickier to make sure you have the right amount of arguments in your call to set up the given component
* Components:
*   Each component has a unique Component ID (Transform_Component, Sprite_Component, etc...)
*   Each component has a virtual method for initialization, update, and draw
*   init() is called when a component is added to the entity, and afterwards update() and draw() are called ever frame
*	  Not all components do anything when draw() is called, but with the ECS, the manager can and draw() all entities and their components with one line of code (manager->draw()) when the draw() method is present for all components
* Manager:
*   In order to have entities and their components, there has to be a Manager to control them
*   The Manager class stores a vector of all active entities, as well as an array containing a vector of all entities of each Group type
*     This allows for easy access of all entities of a certain Group type, such as player, enemy, or goal
*   The manager is where the overarching update() and draw() methods exist. When they are called, each entity it is managing calls its respective methods, and then each component of that entity in turn
*   The refresh method updates the entities and grouped_entities collections
*     First, it checks all grouped entity vectors to check if any of its contents are no longer active, or no longer contains the given group, and removes them from that grouped_entities vector
*     Next, it checks whether an entity is no longer active in the entities vector, and removes it from the vector if it is
*     This method serves to free up any space used by inactive entities
*   The add_to_group(), get_group(), and add_entity() methods do exactly what you would expect given their names, adding and retrieving information from its stored collections
*/


class Entity;
class Manager;

struct Component {
	Entity* entity;

	virtual void init() {}
	virtual void update() {}
	virtual void draw() {}
};

constexpr std::size_t max_components = 10;
constexpr std::size_t max_groups = 10;

using Component_Bit_Set = std::bitset<max_components>;
using Component_ID = std::size_t;
using Group = std::size_t;
using Component_Array = std::array<Component*, max_components>;

using Group_Bit_Set = std::bitset<max_groups>;

inline Component_ID get_new_component_type_ID() {
	static Component_ID last_ID = 0u;
	return last_ID++;
}

template <typename T> inline Component_ID get_component_type_ID() noexcept {
	static Component_ID type_ID = get_new_component_type_ID();
	return type_ID;
}


class Entity {
private:
	Manager& manager;
	bool active = true;
	std::vector<std::unique_ptr<Component>> components;

	Component_Array component_array;
	Component_Bit_Set component_bit_set;
	Group_Bit_Set group_bit_set;

public:
	Entity(Manager& mManager) : manager(mManager) {}
	void update() {
		for (auto& c : components) c->update();
	}
	void draw() {
		for (auto& c : components) c->draw();
	}
	bool is_active() const { return active; }
	void destroy() { active = false; }

	bool has_group(Group mGroup) {
		return group_bit_set[mGroup];
	}

	void add_group(Group mGroup);
	void del_group(Group mGroup) {
		group_bit_set[mGroup] = false;
	}

	template <typename T> bool has_component() const {
		return component_bit_set[get_component_type_ID<T>()];
	}

	template <typename T, typename... TArgs>
	T& add_component(TArgs&&... mArgs) {
		T* c(new T(std::forward<TArgs>(mArgs)...));
		c->entity = this;
		std::unique_ptr<Component> uPtr{ c };
		components.emplace_back(std::move(uPtr));

		component_array[get_component_type_ID<T>()] = c;
		component_bit_set[get_component_type_ID<T>()] = true;

		c->init();
		return *c;
	}

	template<typename T> T& get_component() const {
		auto ptr(component_array[get_component_type_ID<T>()]);
		return *static_cast<T*>(ptr);
	}
};

class Manager {
private:
	std::vector<std::unique_ptr<Entity>> entities;
	std::array<std::vector<Entity*>, max_groups> grouped_entities;
public:
	void update() {
		for (auto& e : entities) e->update();
	}

	void draw() {
		for (auto& e : entities) e->draw();
	}

	void refresh() {
		for (auto i(0u); i < max_groups; i++) {
			auto& v(grouped_entities[i]);
			v.erase(
			std::remove_if(std::begin(v), 
			std::end(v),
			[i](Entity* mEntity) {
				return !mEntity->is_active() || !mEntity->has_group(i);
				}),
			std::end(v));
		}

		entities.erase(
			std::remove_if(std::begin(entities), 
			std::end(entities),
			[](const std::unique_ptr<Entity>& mEntity) {
				return !mEntity->is_active();
			}),
			std::end(entities));
	}

	void add_to_group(Entity* mEntity, Group mGroup) {
		grouped_entities[mGroup].emplace_back(mEntity);
	}

	std::vector<Entity*>& get_group(Group mGroup) {
		return grouped_entities[mGroup];
	}

	Entity& add_entity() {
		Entity* e = new Entity(*this);
		std::unique_ptr<Entity> uPtr{ e };
		entities.emplace_back(std::move(uPtr));
		return *e;
	}
};