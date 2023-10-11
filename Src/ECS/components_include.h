#pragma once

//If a file only needs one or two components, those component files can be included directly instead. However, for files using many/all components, it is easier to handle all #include's in one file to cut down on #include statements in each file

#include "ECS.h"
#include "transform_component.h"
#include "sprite_component.h"
#include "keyboard_controller.h"
#include "collider_component.h"
#include "tile_component.h"