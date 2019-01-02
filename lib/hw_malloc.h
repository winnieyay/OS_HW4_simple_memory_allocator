#ifndef HW_MALLOC_H
#define HW_MALLOC_H


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

typedef unsigned long long ull;

struct chunk_info {
    unsigned int prev_chunk_size : 31;
    unsigned int allocated: 1;
    unsigned int current_chunk_size : 31;
    unsigned int mmap_flag: 1;
};

struct chunk {
    struct chunk *prev;
    struct chunk *next;
    struct chunk_info size_and_flag;
};

void Push_mmap(struct chunk *new);
void Push(struct chunk* new,int i);
void Pop_target(ull addr, int bin);
void split(ull addr, int i);
void merge(ull addr);
void *hw_malloc(size_t bytes);
int hw_free(ull mem);
void *get_start_sbrk(void);
ull Pop(int bin);
void init_bin();
void print_bin(int i);
extern struct chunk* bin_head[];
extern struct chunk* bin_tail[];
extern int bin_num[];
extern void* heap;
extern int mmap_num;
extern struct chunk*HEAD;
extern struct chunk*TAIL;

#endif
