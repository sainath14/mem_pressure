all: memory_mmap.c
	gcc memory_mmap.c -o mmap -lpthread

clean:
	rm mmap
