#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

#include <sched.h>

#define VIRTUAL_MEMORY_CHUNK 4*1024*1024
#define NUM_PROCESSORS  4

void* put_pressure_on_mem (void* args);
void check_my_affinity(void);

int main (int argc, char *argv[])
{

  pthread_t threads[NUM_PROCESSORS];
  pthread_attr_t attr;
  cpu_set_t cpus;
  int index;

  pthread_attr_init(&attr);


  for (index = 0; index < NUM_PROCESSORS; index++) {
      CPU_ZERO(&cpus);
      CPU_SET(index, &cpus);
      pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
      pthread_create(&threads[index], &attr, put_pressure_on_mem, NULL);
  }

  for (index = 0; index < NUM_PROCESSORS; index++) {
      pthread_join(threads[index], NULL);
  }
}

void check_my_affinity(void)
{

  pthread_t thread;
  cpu_set_t cpus;

  thread = pthread_self();
  pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpus);
  printf("My affinity 0x%x\n", cpus);  

}

void* put_pressure_on_mem (void* args)
{
    void * memory;
    volatile int loop = 1;
    int num_64byte_lines = 0;
    int line;

    srand(0);

    check_my_affinity();

    while (loop) {
        sleep(10);
        memory = mmap (NULL, VIRTUAL_MEMORY_CHUNK, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
        if (memory == MAP_FAILED) {
           printf("memory allocation failed \n");
           printf("last error %x\n", errno);
           return 0;
        } else {
           printf("allocated 20 MB of virtual space\n");
           printf("memory address %lx\n", memory);
        }

        if (mlock(memory, VIRTUAL_MEMORY_CHUNK)) {
           printf("memory locking failed \n");
           free(memory);
           continue;
        } else {
           printf("mlocked %x MB of virtual space \n", VIRTUAL_MEMORY_CHUNK);
        }

        memset(memory, 0, VIRTUAL_MEMORY_CHUNK);
        num_64byte_lines = VIRTUAL_MEMORY_CHUNK/64;
        for (line =0; line < num_64byte_lines; line++) {
              *(unsigned long *)((char *)memory + line*64) = ((unsigned long)memory + line*64);
//            *((char *)memory + line*64) = rand();
//              printf("memory location %lx - value %lx \n", ((char *)memory + line*64), *(unsigned long *)((char *)memory + line*64));
//              sleep(5);
        }
    }
    return 0;

}
