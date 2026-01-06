#include "entity-manager.h"
#include <time.h>

entity_manager_t* entity_manager_init(linear_arena_t *arena){
  
  return (entity_manager_t*)linear_arena_alloc(arena, sizeof(entity_manager_t));
}

entity_t create_entity(slab_arena_t* arena, entity_manager_t* em, uint64_t element_id){

  seed_xorshift((uint32_t)time(NULL));
  
  position_comp_t *s = (position_comp_t*)slab_arena_alloc(arena, sizeof(position_comp_t));
  s->x = rand_range(em->game_rect->left, em->game_rect->right); 
  s->y = rand_range(em->game_rect->top, em->game_rect->bottom);     

  velocity_comp_t *v = (velocity_comp_t*)slab_arena_alloc(arena, sizeof(velocity_comp_t));
  v->dx = rand_range(MIN_VEL, MAX_VEL);
  v->dy = rand_range(MIN_VEL, MAX_VEL);
  
  shape_comp_t *shape = (shape_comp_t*)slab_arena_alloc(arena, sizeof(shape_comp_t));
  shape->shape_type = CIRCLE;
  shape->shape = (shape_t){ .circle = (circle_t){ .radius = rand_range(MIN_RAD, MAX_RAD)}}; 
  shape->color = (color_t){ .r = (uint32_t)rand_range(0, 255), .b = (uint32_t)rand_range(0, 255), .g = (uint32_t)rand_range(0, 255), .a = 255 };

  em->entities[em->count] = element_id; 
  em->positions[em->count] = s;
  em->velocities[em->count] = v;
  em->shapes[em->count] = shape;
  

  em->count++;

  return (entity_t)(em->count - 1);
}


void upd_entity_pos(entity_manager_t* em){

  for(int i = 0; i < em->count; i++){

    float *x = &em->positions[i]->x;
    float *y = &em->positions[i]->y;
    float *dx = &em->velocities[i]->dx;
    float *dy = &em->velocities[i]->dy;
    float *r = &em->shapes[i]->shape.circle.radius;

    if (*x + *dx + *r > em->game_rect->right || *x + *dx - *r < em->game_rect->left) {
      *dx = -*dx;
    }

    if (*y + *dy + *r > em->game_rect->bottom || *y + *dy - *r < em->game_rect->top) {
      *dy = -*dy;
    }

    *x += *dx;
    *y += *dy;  
  }
  
}

