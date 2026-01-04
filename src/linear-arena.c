#include "linear-arena.h"
#include <assert.h>

linear_arena_t *linear_arena_init(uint64_t size, uint64_t alignment) {

  assert(alignment >= sizeof(void *) &&
         "ERROR: alignment must be greater or equal to 8.");

  size_t header_size = ALIGN(sizeof(linear_arena_t), alignment);
  size_t data_size = ALIGN(size, alignment);
  size_t total_size = ALIGN(data_size + header_size, alignment);

  linear_arena_t *arena = (linear_arena_t*)VirtualAlloc(NULL, total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  arena->size = total_size;
  arena->offset = header_size;
  arena->alignment = alignment;
  
  return arena;
}

void *linear_arena_alloc(linear_arena_t* arena, uint64_t commit_size) {

  commit_size = ALIGN(commit_size, arena->alignment);
  
  assert(arena->offset + commit_size <= arena->size && "ERROR: arena is full.");
  
  void* p = (void*)((char*)arena + arena->offset);

  arena->offset += commit_size;

  return p;
}

void linear_arena_free(linear_arena_t* arena) {
  arena->offset = 0;
}

void linear_arena_destroy(linear_arena_t* arena) {
  assert(!VirtualFree(arena, 0, MEM_RELEASE) && "ERROR: VirtualFree failed.");
}
