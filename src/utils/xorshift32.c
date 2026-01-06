#include "xorshift32.h"

static uint32_t state;

void seed_xorshift(uint32_t seed) {
  if (seed == 0)
    seed = 2463534242;
  state = seed;
}

uint32_t xorshift32(void) {
  uint32_t x = state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  state = x;
  return x;
}

float rand_range(float min, float max) {
  return min + ((xorshift32() & 0xFFFFFFFFu)/4294967296.0f) * (max - min);
}
