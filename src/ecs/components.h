#pragma once

typedef struct position_comp {
  float x;
  float y;
} position_comp_t;

typedef struct velocity_comp {
  float dx;
  float dy;
} velocity_comp_t;

typedef enum SHAPE { RECTANGLE, CIRCLE } SHAPE;

typedef struct rectangle { float width; float height;  } rectangle_t;
typedef struct circle { float radius;  } circle_t;

typedef union shape {
  rectangle_t rectangle;
  circle_t circle;
} shape_t;

typedef union shape_comp {
  SHAPE shape_type;
  shape_t shape;
    
} shape_comp_t;

