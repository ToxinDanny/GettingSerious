#pragma once

#include <stdint.h>
#include "linear-arena.h"

#define QUEUE_DEF_SIZE (1<<13)

typedef enum COMMAND { CREATE_CMD, CREATE_N_CMD, DELETE_CMD, DESTROY_CMD } COMMAND;

typedef struct command_queue {
  COMMAND cmd[QUEUE_DEF_SIZE];
  int head;
  int tail;
  int count;
} command_queue_t;

command_queue_t* cmd_queue_init(linear_arena_t* arena);
void enqueue_cmd(command_queue_t *queue, COMMAND cmd);
COMMAND dequeue_cmd(command_queue_t *queue);
