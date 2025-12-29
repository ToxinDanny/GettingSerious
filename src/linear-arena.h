#include <stdint.h>
#include <Windows.h>
#include <stdio.h>

typedef struct linear_arena linear_arena_t;

typedef struct linear_arena {

  void* p;
  uint64_t size;
  uint64_t offset;
  uint64_t alignment; 
  linear_arena_t* next;
  
} linear_arena_t;

linear_arena_t *linear_arena_init(uint64_t size, uint64_t alignment);
void *linear_arena_alloc(linear_arena_t **arena, uint64_t offset);
void linear_arena_free(linear_arena_t* arena);
void linear_arena_destroy(linear_arena_t* arena);
