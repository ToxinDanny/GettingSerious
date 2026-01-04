#include "command_queue.h"
#include <assert.h>

command_queue_t* cmd_queue_init(linear_arena_t* arena){

  assert(arena != NULL && "ERROR: arena is NULL.");
  
  command_queue_t* queue = (command_queue_t*)linear_arena_alloc(arena, sizeof(command_queue_t));

  if (queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
  }

  return queue;
}

void enqueue_cmd(command_queue_t *queue, COMMAND cmd){
  assert(queue != NULL && "ERROR: queue is NULL.");
  assert((cmd >= 0 && cmd <= 3) && "ERROR: cmd is not valid COMMAND.");
  assert(queue->count < QUEUE_DEF_SIZE && "ERROR: queue is full.");

  queue->cmd[queue->tail] = cmd;
  queue->tail = (queue->tail + 1) % QUEUE_DEF_SIZE;
  queue->count++;
}

COMMAND dequeue_cmd(command_queue_t *queue){

  assert(queue != NULL && "ERROR: queue is NULL");
  assert(queue->count > 0 && "ERROR: queue is empty");
  
  COMMAND cmd = queue->cmd[queue->head]; 
  queue->head = (queue->head + 1) % QUEUE_DEF_SIZE;
  queue->count--;
  
  return cmd;
}
