#include "linear-arena.h"

linear_arena_t *linear_arena_init(uint64_t size, uint64_t alignment) {

  size = (size + alignment - 1) & ~(alignment - 1);

  linear_arena_t *arena = (linear_arena_t*)malloc(sizeof(linear_arena_t));

  arena->p = VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

  if(arena->p == NULL){
    printf("Errore: impossibile allocare memoria. (errno: %lu)\n", GetLastError());
    return NULL;
  }

  arena->size = size;
  arena->offset = 0;
  arena->alignment = alignment;
  arena->next = NULL;

  return arena;
}

void *linear_arena_alloc(linear_arena_t** arena_ref, uint64_t commit_size) {

  linear_arena_t* current = *arena_ref;
  commit_size = (commit_size + current->alignment - 1) & ~(current->alignment - 1);
  
  if(current->offset + commit_size > current->size){
    current->next = linear_arena_init(current->size, current->alignment);
    *arena_ref = current->next;
  }

  void* p = (void*)((char*)current->p + current->offset); 

  current->offset += commit_size;

  return p;
}

void linear_arena_free(linear_arena_t* arena) {
  if(arena->next)
    linear_arena_free(arena->next);

  arena->offset = 0;
}

void linear_arena_destroy(linear_arena_t* arena) {

  if(arena->next)
    linear_arena_destroy(arena->next);

  if(!VirtualFree(arena->p, 0, MEM_RELEASE)){
    printf("Errore: impossibile deallocare la memoria. (errno: %lu)\n", GetLastError());
    return;
  }

  free(arena);
}
