#pragma once

#include <stdint.h>
#include <Windows.h>

#define ALIGN(x, a) (assert(((a) != 0 && ((a) & ((a) - 1)) == 0) && "ERROR: alignment must be a power-of-2 number."), (((x) + (a) - 1) & ~((a) - 1)))

typedef struct linear_arena linear_arena_t;
typedef struct linear_arena {

  uint64_t alignment;
  uint64_t size;
  uint64_t offset;
  
} linear_arena_t;

linear_arena_t *linear_arena_init(uint64_t size, uint64_t alignment);
void *linear_arena_alloc(linear_arena_t *arena, uint64_t offset);
void linear_arena_free(linear_arena_t* arena);
void linear_arena_destroy(linear_arena_t* arena);
