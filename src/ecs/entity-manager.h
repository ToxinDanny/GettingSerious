#pragma once

#include <stdint.h>
#include "ecs.h"
#include "components.h"
#include "../slab-arena.h"
#include "../linear-arena.h"
#include "../utils/xorshift32.h"

typedef struct window_rect {
  float left;
  float right;
  float top;
  float bottom;
  BOOL is_initialized;
} window_rect_t;

typedef uint64_t entity_t;

typedef struct entity_manager {

  window_rect_t *game_rect;
  int count;
  entity_t entities[10000];
  position_comp_t *positions[10000];
  velocity_comp_t *velocities[10000];
  shape_comp_t *shapes[10000];
  
} entity_manager_t;

entity_manager_t* entity_manager_init(linear_arena_t* arena);
entity_t create_entity(slab_arena_t* arena, entity_manager_t* em, uint64_t element_id);
void upd_entity_pos(entity_manager_t* em);
void render_entities(entity_manager_t* em);
