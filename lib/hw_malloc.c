#include <stdlib.h>
#include <stdio.h>
#include "hw_malloc.h"
#define mmap_threshold 32768

struct chunk* bin_head[11];
struct chunk* bin_tail[11];
int bin_num[11];
struct chunk *HEAD = NULL;
struct chunk *TAIL = NULL;
int mmap_num;


void init_bin()
{
    for(int i = 0; i < 11; i++) {
        bin_num[i] = 0;
        bin_head[i] = NULL;
        bin_tail[i] = NULL;
    }
}

void Push(struct chunk* new,int i)
{
    if(bin_head[i] == NULL) {
        bin_head[i] = new;
        bin_tail[i] = new;
        new->prev = bin_head[i];
        new->next = bin_head[i];
    } else {
        new->next = bin_head[i];
        new->prev = bin_tail[i];
        bin_head[i]->prev = new;
        bin_tail[i]->next = new;
        bin_tail[i] = new;
    }
    bin_num[i]++;
}

int Pop_mmap(ull addr)
{

    if(mmap_num == 0) {
        return 0;
    } else {
        struct chunk* ptr = HEAD;
        int flag = 0;
        for(int x=0; x<mmap_num; x++) {
            if((ull)ptr == addr) {
                flag = 1;
                break;
            }
            ptr = ptr->next;
        }
        if(flag) {
            if(ptr == HEAD && ptr == TAIL) {
                HEAD = TAIL = NULL;
            } else {
                if(ptr == HEAD) {
                    HEAD = ptr->next;
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                } else if(ptr == TAIL) {
                    TAIL = ptr->prev;
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                } else {
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                }
            }
            mmap_num--;
        } else {
            return 0;
        }
    }
    return 1;
}


ull Pop(int bin)
{
    ull find_min;
    if(bin_num[bin] == 0) {
        return 0;
    } else {

        struct chunk* ptr = bin_head[bin];
        struct chunk* min_chunk = NULL;

        find_min = (ull)ptr;
        min_chunk = ptr;

        for(int i=0; i<bin_num[bin]; i++) {
            if(find_min > (ull)ptr) {
                find_min = (ull)ptr;
                min_chunk = ptr;
            }
            ptr = ptr->next;
        }
        if( min_chunk == bin_head[bin] &&  min_chunk == bin_tail[bin]) {
            bin_head[bin] = bin_tail[bin] = NULL;
        } else {
            if( min_chunk == bin_head[bin]) {
                bin_head[bin] =  min_chunk->next;
                min_chunk->prev->next =  min_chunk->next;
                min_chunk->next->prev =  min_chunk->prev;

            } else if( min_chunk == bin_tail[bin]) {
                bin_tail[bin] =  min_chunk->prev;
                min_chunk->prev->next =  min_chunk->next;
                min_chunk->next->prev =  min_chunk->prev;
            } else {
                min_chunk->prev->next =  min_chunk->next;
                min_chunk->next->prev =  min_chunk->prev;
            }
        }
        bin_num[bin] -- ;
    }

    return find_min;
}

void Push_mmap(struct chunk *new)
{

    if(HEAD == NULL) {
        HEAD = new;
        HEAD->prev = HEAD;
        HEAD->next = HEAD;
        TAIL = HEAD;
    } else {

        struct chunk *ptr = HEAD;
        if(ptr->size_and_flag.current_chunk_size < new->size_and_flag.current_chunk_size) {
            for (int i = 0; i < mmap_num; ++i) {
                if(ptr->size_and_flag.current_chunk_size > new->size_and_flag.current_chunk_size) {
                    break;
                }
                ptr = ptr->next;
            }
            if(ptr == HEAD) {
                //printf("1\n");
                TAIL->next = new;
                HEAD->prev = new;
                new -> next = HEAD;
                new -> prev = TAIL;
                TAIL = new;
            } else {
                //printf("2\n");
                new->next = ptr;
                new->prev = ptr->prev;
                ptr->prev->next = new;
                ptr->prev = new;

            }

        } else {
            //printf("3\n");
            new->prev = TAIL;
            new->next = HEAD;
            HEAD->prev = new;
            TAIL->next = new;
            HEAD = new;
        }
    }
    mmap_num++;
}

void Pop_target(ull addr, int bin)
{
    if(bin_num[bin] == 0) {
        return;
    } else {

        int flag = 0;
        struct chunk* ptr = bin_head[bin];

        for(int i=0; i<bin_num[bin]; i++) {
            if((ull)ptr == addr) {
                flag = 1;
                break;
            }
            ptr = ptr->next;
        }

        if(flag) {
            if(ptr == bin_head[bin] && ptr == bin_tail[bin]) {

                bin_head[bin] = NULL;
                bin_tail[bin] = NULL;
            } else {
                if(ptr == bin_head[bin]) {
                    bin_head[bin] = ptr->next;
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                } else if(ptr == bin_tail[bin]) {
                    bin_tail[bin] = ptr->prev;
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                } else {
                    ptr->prev->next = ptr->next;
                    ptr->next->prev = ptr->prev;
                }
            }
            bin_num[bin]--;
        } else {
            return ;
        }
    }
}
void split(ull addr, int i)
{


    ull first_addr = addr;
    ull second_addr = addr + (1 << (i - 1 + 5));

    struct chunk* first_new = (struct chunk *)first_addr;
    first_new->prev = NULL;
    first_new->next = NULL;
    first_new->size_and_flag.prev_chunk_size = ((struct chunk *)addr)->size_and_flag.prev_chunk_size;
    first_new->size_and_flag.allocated = 0;
    first_new->size_and_flag.current_chunk_size = (i - 1 + 5);
    first_new->size_and_flag.mmap_flag = 0;


    struct chunk* second_new = (struct chunk *)second_addr;
    second_new->prev = NULL;
    second_new->next = NULL;
    second_new->size_and_flag.prev_chunk_size = (i - 1 + 5);
    second_new->size_and_flag.allocated = 0;
    second_new->size_and_flag.current_chunk_size = (i - 1 + 5);
    second_new->size_and_flag.mmap_flag = 0;


    Push(first_new, i - 1);
    Push(second_new, i - 1);

    ull next_addr = addr + (1 << (i + 5));
    ((struct chunk *)next_addr)->size_and_flag.prev_chunk_size = i - 1 + 5;
}
void merge(ull addr)
{


    struct chunk *mergec = (struct chunk *)addr;
    if(mergec->size_and_flag.current_chunk_size >= 15)
        return;
    else {

        ull prev_addr = addr - (1 << mergec->size_and_flag.prev_chunk_size);
        ull next_addr = addr + (1 << mergec->size_and_flag.current_chunk_size);

        struct chunk *prev_merge = (struct chunk *)prev_addr;
        struct chunk *next_merge = (struct chunk *)next_addr;

        if(prev_merge->size_and_flag.current_chunk_size == mergec->size_and_flag.current_chunk_size && prev_merge->size_and_flag.allocated == 0 && mergec->size_and_flag.prev_chunk_size != 0) {

            Pop_target(prev_addr, prev_merge->size_and_flag.current_chunk_size - 5);
            Pop_target(addr, mergec->size_and_flag.current_chunk_size - 5);

            prev_merge->size_and_flag.current_chunk_size ++;
            Push(prev_merge, prev_merge->size_and_flag.current_chunk_size - 5);
            next_merge->size_and_flag.prev_chunk_size = prev_merge->size_and_flag.current_chunk_size;
            //mergec->size_and_flag.allocated = 0;
            merge(prev_addr);
        } else if(mergec->size_and_flag.current_chunk_size == next_merge->size_and_flag.current_chunk_size && next_merge->size_and_flag.allocated == 0 && next_addr != (ull)heap + 64 * 1024 ) {

            Pop_target(addr, mergec->size_and_flag.current_chunk_size - 5);
            Pop_target(next_addr, next_merge->size_and_flag.current_chunk_size - 5);

            mergec->size_and_flag.current_chunk_size ++;
            Push(mergec, mergec->size_and_flag.current_chunk_size - 5);
            ull next_2_addr = next_addr + (1 << next_merge->size_and_flag.current_chunk_size);
            //if(next_2_addr != (ull)heap + 64 * 1024)
            ((struct chunk *)next_2_addr)->size_and_flag.prev_chunk_size = mergec->size_and_flag.current_chunk_size;
            //next_merge->size_and_flag.allocated = 0;
            merge(addr);
        }

    }
}
void print_mmap_alloc_list()
{
    struct chunk* ptr = HEAD;
    if(HEAD != NULL) {
        for(int i=0; i<mmap_num; i++) {
            printf("0x%012llx", (ull)ptr - (ull)heap);
            printf("------%d\n", ptr->size_and_flag.current_chunk_size+24);
            ptr = ptr->next;
        }
    }
}

void print_bin(int i)
{
    //printf("IN BIN %d\n",i);
    if(bin_num[i] != 0) {
        //printf("IN BIN\n");
        struct chunk* ptr = bin_head[i];

        if(bin_num[i] == 1) {
            int size;
            size = 1 << ptr->size_and_flag.current_chunk_size;
            printf("0x%08llx",(ull)ptr - (ull)heap );
            printf("-------");
            printf("%d\n",size);
        }

        else {
            for(int j=0; j<bin_num[i]; j++) {
                int size;
                size = 1 << ptr->size_and_flag.current_chunk_size;
                printf("0x%08llx",(ull)ptr - (ull)heap );
                printf("-------");
                printf("%d\n",size);
                ptr = ptr ->next;
            }
        }
    }
    return;
}
//fix
void *hw_malloc(size_t bytes)
{

    void *address = NULL;

    if(bytes+24 > mmap_threshold) {
        address = mmap(NULL, bytes, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

        struct chunk* init = (struct chunk *)address;
        init->prev = NULL;
        init->next = NULL;
        init->size_and_flag.prev_chunk_size = 0;
        init->size_and_flag.allocated = 1;
        init->size_and_flag.current_chunk_size = bytes;
        init->size_and_flag.mmap_flag = 1;

        Push_mmap((struct chunk *)address);
    } else {
        int i;
        int record_bin;
        for (i = 0; i < 11; ++i) {
            if((1 << (i + 5)) >= (bytes + 24) && (bin_num[i] != 0)) {
                record_bin = i;
                break;
            }
        }



        while(1) {
            if(((1 << (record_bin + 5)) >= (bytes + 24)) && ((1 << (record_bin - 1 + 5)) < (bytes + 24))) {
                address = (void *)Pop(record_bin);
                ((struct chunk *)address)->size_and_flag.allocated = 1;
                break;
            }
            if((1 << (record_bin - 1 + 5)) >= (bytes + 24) && record_bin > 0) {
                ull split_addr = Pop(record_bin);
                split(split_addr, record_bin);
                record_bin--;
            } else {
                address = (void *)Pop(record_bin);
                ((struct chunk *)address)->size_and_flag.allocated = 1;
                break;
            }

        }
    }
    return (void *)((ull)address + 24);
    //return NULL;

}
int hw_free(ull mem)
{
    ull free_addr = mem - 24;
    struct chunk *free = (struct chunk *)free_addr;
    if(free->size_and_flag.current_chunk_size > 32 * 1024 - 24) {
        if(Pop_mmap(free_addr) == 0) {
            return 0;
        }
        int check;
        check = munmap((void *)free_addr, free->size_and_flag.current_chunk_size);
        if(check == -1) {
            return 0;
        }
    } else {
        if(free_addr < (ull)heap || free_addr > (ull)heap + 64 * 1024)
            return 0;
        else {

            free->size_and_flag.allocated = 0;
            Push(free, free->size_and_flag.current_chunk_size - 5);
            merge(free_addr);

        }
    }
    return 1;
}
void *get_start_sbrk(void)
{
    return NULL;
}