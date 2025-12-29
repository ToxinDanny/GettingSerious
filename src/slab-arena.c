#include "slab-arena.h"

// TODO:
// 1) Slab fragmentation -> i'm not tracking empty slabs vs partial empty slabs. Must implement empty slabs free logic.
// 2) Static Arena -> Must implement self grow arena (like linear one).
// 3) Thread Safety -> Must transform slab to slub.
// 4) Diagnostics -> Must add slabs counters to arena to track memory.

slab_arena_t *slab_arena_init(uint64_t size, uint64_t alignment) {

  assert(alignment >= sizeof(void*) && "ERROR: alignment must be greater or equal to 8.");
  
  size_t header_size = ALIGN(sizeof(slab_arena_t), alignment);
  size_t data_size = ALIGN(size, alignment);
  size_t total_size = ALIGN(data_size + header_size, alignment);

  slab_arena_t *arena = (slab_arena_t *)VirtualAlloc(NULL, total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
  assert(arena != NULL || "ERROR: Arena allocation failed.");
  
  arena->alignment = alignment;
  arena->size = total_size;
  arena->offset = header_size;
//arena->next = NULL;   for now arena is static. Thinking about make it dynamic. 

  for(int i = 0; i < TOTAL_CACHES; i++){
    InitializeSRWLock(&arena->locks[i]);
  }

  return arena;
}

void *slab_arena_alloc(slab_arena_t *arena, uint64_t commit_size) {
  size_t aligned = ALIGN(commit_size, arena->alignment);
  ULONG index = 0;
  int slice = 0;

  if (aligned < SMALL_CACHES_SIZE_CAP) {
    index = aligned / SMALL_CACHES_SIZE_GROWTH - 1;
    slice = aligned;

  } else {

    assert(aligned < SLAB_SIZE || "ERROR: Slot size > slab size. Rethink objects composition or enlarge slab size.");

    _BitScanReverse(&index, aligned);
    slice = 1 << index;
    index += 8;
  }

  AcquireSRWLockExclusive(&arena->locks[index]);
  
  slab_t *slab = arena->caches[index];

  if (slab == NULL || slab->free_list == NULL) {
    // 1. alloco la slab e la inserisco nella cache
    slab_t* new_slab = (slab_t *)((char *)arena + arena->offset);

    assert(arena->offset + SLAB_SIZE < arena->size && arena->offset || "ERROR: Arena is full. Enlarge arena size or make it dynamic.");
    arena->offset += SLAB_SIZE;

    new_slab->next = arena->caches[index];
    new_slab->prev = NULL;

    if(arena->caches[index] != NULL){
      arena->caches[index]->prev = new_slab;
    }

    arena->caches[index] = new_slab;

    // 2. slicing della slab = inizializzo i next
    uintptr_t start = (uintptr_t)new_slab + sizeof(slab_t);
    uintptr_t end = (uintptr_t)new_slab + SLAB_SIZE;
    slot_t *current = (slot_t *)start;
    
    new_slab->free_list = current;

    while ((uintptr_t)current + slice < end) {
      current->next = (union slot *)((char *)current + slice);
      current = (slot_t *)current->next;
    }

    current->next = NULL;
    slab = new_slab;
  }

  slot_t *allocated_slot = slab->free_list;
  if (allocated_slot != NULL) {
    slab->free_list = (slot_t *)allocated_slot->next;
  }

  ReleaseSRWLockExclusive(&arena->locks[index]);

  return allocated_slot;
}

void slab_arena_free(slab_arena_t* arena, slot_t* slot, size_t slot_size) {

  ULONG index = 0;
  size_t aligned = ALIGN(slot_size, arena->alignment);
  
  if(aligned < SMALL_CACHES_SIZE_CAP)
    index = aligned / SMALL_CACHES_SIZE_GROWTH - 1;
  else {
    _BitScanReverse(&index, aligned);
    index += 8;
  }

  slab_t *current_slab = (slab_t*)((uintptr_t)slot & ~(SLAB_SIZE - 1));

  AcquireSRWLockExclusive(&arena->locks[index]);

  // move free slot to the head of the slab
  slot->next = current_slab->free_list;
  current_slab->free_list = slot;

  // move utilizable slab to the head of the cache
  if(current_slab->prev != NULL) {
    ((slab_t*)current_slab->prev)->next = current_slab->next;
  }

  if(current_slab->next != NULL) {
    ((slab_t *)current_slab->next)->prev = current_slab->prev;
  }

  current_slab->prev = NULL;
  current_slab->next = arena->caches[index];

  if(arena->caches[index] != NULL){
    arena->caches[index]->prev = current_slab;
  }

  arena->caches[index] = current_slab;

  ReleaseSRWLockExclusive(&arena->locks[index]);
}

void slab_arena_destroy(slab_arena_t* arena) {
  BOOL res = VirtualFree(arena, 0, MEM_RELEASE) == TRUE;
  assert(res || "ERROR: Arena deallocation failed.");
}
