#include "ex01_stopwatch.h"

static int* array;
static int stride;

void Elaborate() {
  for (int i = 0; i < 100000 * stride; i += stride) {
    int temp = array[i];
  }
}

void ex01_stopwatch() {

  printf("Starting...\r\n");

  srand(time(NULL));
  printf("srand ok\r\n");
  
  array = (int*)malloc(250000000 * sizeof(int));
  printf("malloc ok\r\n");

  for(int i = 0; i < 250000000; i++){
    array[i] = rand();
  }

  printf("fill array ok\r\n");
  
  FILE* cycles_file;
  cycles_file = fopen("build\\cycles.txt", "w");
  FILE* times_file;
  times_file = fopen("build\\times.txt", "w");

  for (stride = 1; stride < 1024; stride++) {
    uint64_t cycles;
    uint64_t time;

    measure(&cycles, &time, Elaborate, NULL)

    fprintf(times_file, "%d %llu\n", stride, time);
    fprintf(cycles_file, "%d %llu\n", stride, cycles); 
  }

  fclose(times_file);
  fclose(cycles_file);
  free(array);
  printf("Test ended successfully.");
}
