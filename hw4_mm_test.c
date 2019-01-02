#include "lib/hw_malloc.h"
#include "hw4_mm_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define KiB 1024
void* heap = NULL;



int main(int argc, char *argv[])
{
    //fix : init
    //fix fgets
    init_bin();

    heap = sbrk(64 * 1024);

    struct chunk* heap_init = (struct chunk *)heap;
    heap_init->prev = NULL;
    heap_init->next = NULL;
    heap_init->size_and_flag.prev_chunk_size = 0;
    heap_init->size_and_flag.allocated = 0;
    heap_init->size_and_flag.current_chunk_size = 15;
    heap_init->size_and_flag.mmap_flag = 0;

    Push(heap_init, 10);

    struct chunk* heap_init_m = (struct chunk *)((ull)heap + 32 * 1024);
    heap_init_m->prev = NULL;
    heap_init_m->next = NULL;
    heap_init_m->size_and_flag.prev_chunk_size = 15;
    heap_init_m->size_and_flag.allocated = 0;
    heap_init_m->size_and_flag.current_chunk_size = 15;
    heap_init_m->size_and_flag.mmap_flag = 0;

    Push(heap_init_m, 10);

    char buff[50];
    memset(buff,0,sizeof(buff));

    while(fgets(buff,sizeof(buff),stdin)!=NULL) {

        //char buff[50];
        //memset(buff,0,sizeof(buff));
        //fgets(buff,sizeof(buff),stdin);

        if(buff[0] == 'a') {

            char* delim_for_n = "\n";
            char* pch_for_n = NULL;
            pch_for_n = strtok(buff,delim_for_n);

            char* delim = " ";
            char* pch = NULL;
            pch = strtok(pch_for_n,delim);
            pch = strtok(NULL,delim);
            //fix
            ull num = strtoull(pch, NULL, 0);
            ull addr = (ull)hw_malloc(num);
            //printf("0x%llu\n", addr);
            printf("0x%012llx\n", addr - (ull)heap);
            //printf("this is pch %s \n ",pch);
        } else if(buff[0] == 'f') {
            //fix
            char* delim_for_n = "\n";
            char* pch_for_n = NULL;
            pch_for_n = strtok(buff,delim_for_n);

            char* delim = " ";
            char* pch = NULL;
            pch = strtok(pch_for_n,delim);
            pch = strtok(NULL,delim);

            ull num = strtoull(pch, NULL, 0);
            int ans;
            //printf("YO\n");
            ans = hw_free(num + (ull)heap);
            //printf("YO\n");
            if(ans == 1) {
                printf("Success!\n");
            } else {
                printf("Fail!\n");
            }
        } else if(buff[0] == 'p') {
            if(buff[6] == 'b') {
                char* delim_for_n = "\n";
                char* pch_for_n = NULL;
                pch_for_n = strtok(buff,delim_for_n);

                char* delim = " ";
                char* pch = NULL;
                pch = strtok(pch_for_n,delim);
                pch = strtok(NULL,delim);

                char* delim1 = "[";
                char* pch1 = NULL;
                pch1 = strtok(pch,delim1);
                pch1 = strtok(NULL,delim1);

                char* delim2 = "]";
                char* pch2 = NULL;
                pch2 = strtok(pch1,delim2);
                //pch2 = strtok(NULL,delim1);
                //printf("check bin : %s\n",pch2);
                int bin = atoi(pch2);
                //printf("check bin : %d\n",bin);
                print_bin(bin);

            } else if(buff[6] == 'm') {
                print_mmap_alloc_list();
            }

        } else {

            printf("wrong command \n");
        }
        memset(buff,0,sizeof(buff));

    }
    return 0;
}
