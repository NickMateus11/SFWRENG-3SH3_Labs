#include <stdio.h>
#include <stdlib.h>


#define MAX_BYTES   1048576 // 1MB   

typedef enum {
    hole=0,
    process
} mem_type;

typedef enum {
    first_fit=0,
    best_fit,
    worst_fit
} allocation_mode;

typedef struct linkedList{
    mem_type type;
    int start;
    int size;
    struct linkedList *next;
} linkedList;


void allocate(int size, linkedList *list, allocation_mode mode){
    // mode = 0 (first fit), 1 (best fit), 2 (worst fit)

    linkedList *ptr = list;
    switch (mode){
        case first_fit:
            while (ptr != NULL){
                if (ptr->size >= size){ // found fit
                    // create a record for the remaining hole that is created
                    ptr->next = (linkedList *) malloc(sizeof(linkedList));
                    ptr->next->type  = hole;
                    ptr->next->size  = ptr->size - size;
                    ptr->next->start = ptr->start + size;
                    ptr->next->next  = NULL;

                    // occupy this spot
                    ptr->type = process;
                    ptr->size = size;
                    break;
                }
                ptr = ptr->next;
            }
            if (ptr==NULL) { // no spot could be found
                printf("NO Available spot found\n");
                exit(1);
            }
            break;
        case best_fit:
            break;
        case worst_fit:
            break;

        default:
            printf("Not a valid allocation mode!\n");
            exit(1);
            break;
    }
}

void release(int start, linkedList *list){

    linkedList *ptr = list;
    while(ptr->start != start){
        //error check
        if (ptr->start > start || ptr->next == NULL){
            printf("No exact starting point was found\n");
            exit(1);
        }

        ptr = ptr->next;
    }

    // "release" - no automatic compaction
    ptr->type = hole;
}

void compact(linkedList *list){
    // combine adjacent holes
    // free new unused ptrs
}

void status(linkedList *list){
    linkedList *ptr = list;
    while(ptr != NULL){
        printf("TYPE:%d,\t\tSTART:%d,\tSIZE:%d\n",ptr->type, ptr->start, ptr->size);
        ptr = ptr->next;
    }
    printf("\n");
}

int main(int argc, char ** argv){

    // int *memory_region = calloc(MAX_BYTES, sizeof(int)); // don't actuall need to allocate memory - just use the maintainer
    linkedList maintainer = {
        .type=hole,
        .start=0,
        .size=MAX_BYTES,
        .next=NULL
    };


    allocate(8, &maintainer, first_fit);
    allocate(1024, &maintainer, first_fit);
    status(&maintainer);

    release(8, &maintainer);
    status(&maintainer);

    compact(&maintainer);

    return 0;
}