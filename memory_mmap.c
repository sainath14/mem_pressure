#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#define VIRTUAL_MEMORY_CHUNK 512*1024*1024
#define NUM_PROCESSORS  4

int main (int argc, char *argv[])
{

    void * memory;
    volatile int loop = 1;
    int num_64byte_lines = 0;
    int line;

    srand(0);

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
           printf("mlocked 512 MB of virtual space \n");
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
