#include <Windows.h>
#include <stdint.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#define SMALL_CACHES_SIZE_GROWTH 8
#define SMALL_CACHES 16
#define SMALL_CACHES_SIZE_CAP SMALL_CACHES_SIZE_GROWTH * SMALL_CACHES
#define BIG_CACHES 5
#define TOTAL_CACHES SMALL_CACHES + BIG_CACHES
#define SLAB_SIZE (1<<12)

#define ALIGN(x, a) (assert(((a) != 0 && ((a) & ((a) - 1)) == 0) && "ERROR: alignment must be a power-of-2 number."), (((x) + (a) - 1) & ~((a) - 1)))

typedef union slot {
  union slot* next;  
} slot_t;

typedef struct slab {
  slot_t *free_list;
  struct slab* next;
  struct slab* prev;
  
} slab_t;

typedef struct slab_arena {
 
  uint64_t alignment;
  uint64_t size;
  uint64_t offset;
//  struct slab_arena_t* next;
  
  slab_t* caches[TOTAL_CACHES];
  SRWLOCK locks[TOTAL_CACHES];

} slab_arena_t;

slab_arena_t *slab_arena_init(uint64_t size, uint64_t alignment);
void *slab_arena_alloc(slab_arena_t* arena, uint64_t commit_size);
void slab_arena_free(slab_arena_t* arena, slot_t* slot, size_t slot_size);
void slab_arena_destroy(slab_arena_t* arena);
