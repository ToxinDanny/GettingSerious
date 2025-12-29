#include "slab-arena.h"

int main(){

  slab_arena_t* arena = slab_arena_init(1<<20, 4);
  slot_t* slot = slab_arena_alloc(arena, 24);
}
