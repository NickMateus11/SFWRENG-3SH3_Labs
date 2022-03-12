#include <stdio.h>
#include <stdlib.h>


#define MAX_BYTES           1048576 // 1MB  
#define MIN_BLOCK           4096    // 4KB 
#define MAX_BLOCK_FACTOR    25      // 25 * 4096 = 100KB

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

// function to split a block into an allocated and remaining hole partition
void _occupy_block(linkedList *list_item, int size){
    if (list_item->size > size){ // if remaining hole is > 0
        // create a record for the remaining hole that is created
        list_item->next = (linkedList *) malloc(sizeof(linkedList));
        list_item->next->type  = hole;
        list_item->next->size  = list_item->size - size;
        list_item->next->start = list_item->start + size;
        list_item->next->next  = NULL;
    }

    // occupy this spot
    list_item->type = process;
    list_item->size = size;
}

int allocate(int size, linkedList *list, allocation_mode mode){
    if (mode == first_fit){
        linkedList *ptr = list;
        while (ptr != NULL){
            if (ptr->size >= size && ptr->type==hole){ // found fit
                _occupy_block(ptr, size);
                break;
            }
            ptr = ptr->next;
        }
        if (ptr==NULL) { // no spot could be found
            printf("NO Available spot found\n");
            return 0;
        }
    }

    else if (mode == best_fit){
        linkedList *best_block_ptr=NULL, *ptr=list;
        int best_block_size = MAX_BYTES+1;
        while(ptr != NULL){
            if ((ptr->type)==hole && (ptr->size)>=size && (ptr->size)<best_block_size) {// better block found
                best_block_ptr = ptr;
                best_block_size = ptr->size;
            }
            ptr = ptr->next;
        }
        if (best_block_ptr == NULL) { // nothing found
            printf("NO Available spot found\n");
            return 0;
        }
        // occupy best block
        _occupy_block(best_block_ptr, size);
    }

    else if (mode == worst_fit){
        linkedList *worst_block_ptr=NULL, *ptr=list;
        int worst_block_size = 0;
        while(ptr != NULL){
            if ((ptr->type)==hole && (ptr->size)>=size && (ptr->size)>worst_block_size) {// worse block found
                worst_block_ptr = ptr;
                worst_block_size = ptr->size;
            }
            ptr = ptr->next;
        }
        if (worst_block_ptr == NULL) { // nothing found
            printf("NO Available spot found\n");
            return 0;
        }
        // occupy best block
        _occupy_block(worst_block_ptr, size);
    }

    else{
        printf("Not a valid allocation mode!\n");
        exit(1);
    }

    return 1;
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

    // free newly unused ptrs
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

    // int *memory_region = calloc(MAX_BYTES, sizeof(int)); // don't actually need to allocate memory - just use the maintainer
    linkedList maintainer = {
        .type=hole,
        .start=0,
        .size=MAX_BYTES,
        .next=NULL
    };

    srand(1); // seed

    // fill memory
    int bytes_filled = 0;
    int fill_size;
    while(bytes_filled < MAX_BYTES){
        fill_size = (rand()%MAX_BLOCK_FACTOR + 1) * MIN_BLOCK; 
        int res = allocate(fill_size, &maintainer, first_fit);
        if (res) {
            bytes_filled+=fill_size;
        }
    }

    status(&maintainer);

    return 0;
}