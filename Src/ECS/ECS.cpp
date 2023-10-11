#include "ECS.h"

//Needs a definition for manager, so the simplest solution was put it in a different file than the one defining Manager...

void Entity::add_group(Group mGroup) {
	group_bit_set[mGroup] = true;
	manager.add_to_group(this, mGroup);
}