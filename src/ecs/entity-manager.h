#pragma once

#include <stdint.h>
#include "components.h"
#include "slab-arena.h"

typedef uint64_t entity;

typedef struct entity_manager {

  entity* entities;
  position_comp_t* positions;
  velocity_comp_t* velocities;
  shape_comp_t* shapes;
  
} entity_manager_t;

entity create_entity(slab_arena_t* arena, entity_manager_t* em);
void upd_entity_pos(entity e, position_comp_t pos);
