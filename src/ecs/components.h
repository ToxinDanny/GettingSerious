#pragma once

#include <stdint.h>

typedef struct position_comp {
  float x;
  float y;
} position_comp_t;

#define MIN_VEL -10.0f
#define MAX_VEL 10.0f

typedef struct velocity_comp {
  float dx;
  float dy;
} velocity_comp_t;

typedef enum SHAPE { RECTANGLE, CIRCLE } SHAPE;

typedef struct rectangle { float width; float height;  } rectangle_t;

#define MIN_RAD 5
#define MAX_RAD 20
typedef struct circle { float radius;  } circle_t;

typedef union shape {
  rectangle_t rectangle;
  circle_t circle;
} shape_t;

typedef struct color {
  uint8_t r;  
  uint8_t g;
  uint8_t b;
  float a;  
} color_t;

typedef struct shape_comp {
  color_t color;
  SHAPE shape_type;
  shape_t shape;
    
} shape_comp_t;

