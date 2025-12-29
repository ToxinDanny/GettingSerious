#include "ex02_linear-memarena.h"

void classic_alloc(){

  uint32_t **temp = (uint32_t **)malloc(sizeof(uint32_t *) * (1 << 20));

  for (int i = 0; i < (1 << 20); i++) {
    temp[i] = malloc(sizeof(uint32_t) * 8);    
  }

  for (int i = 0; i < (1 << 20); i++) {
    free(temp[i]);
  }

  free(temp);
}

void memarena_alloc(){
  linear_arena_t *arena = linear_arena_init(1<<20, 4);
  linear_arena_t *p = arena;
  
  for (int i = 0; i < (1 << 20); i++) {
    void* temp = linear_arena_alloc(&p, sizeof(uint32_t) * 8);
  }

  linear_arena_free(arena);
}

void ex02_memarena(){

  uint64_t cc, mt;

  measure(&cc, &mt, classic_alloc, NULL);

  printf("Classic Malloc: %llu cycles,  %llu ms\n", cc, mt);

  measure(&cc, &mt, memarena_alloc, NULL);

  printf("MemArena alloc: %llu cycles,  %llu ms", cc, mt);
}

