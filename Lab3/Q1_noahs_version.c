#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
    int num;
    struct linkedList *next;
} linkedList;

void status(linkedList *list, int* byte_counter){
    linkedList *ptr = list;
    while(ptr != NULL){
        printf("TYPE:%d,\t\tSTART:%d,\tSIZE:%d\n",ptr->type, ptr->start, ptr->size);
        ptr = ptr->next;
    }
    printf("\n");
    printf("bytes filled: %d\n\n", *byte_counter);
}

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

int allocate(int size, linkedList *list, allocation_mode mode, int* byteTest){
    if (mode == first_fit){
        linkedList *ptr = list;
        while (ptr != NULL){
            if (ptr->size >= size && ptr->type==hole){ // found fit
                _occupy_block(ptr, size);
                list->num++;
                break;
            }
            ptr = ptr->next;
        }
        if (ptr==NULL) { // no spot could be found
            ptr = list;
            status(list, byteTest);
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
        list->num++;
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
        list->num++;
    }

    else{
        printf("Not a valid allocation mode!\n");
        exit(1);
    }

    return 1;
}

void release(int start, linkedList *list, int* byte_counter){

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
    printf("type: %d\n", ptr->type);
    printf("bytes filled: %d\n", *byte_counter);
    ptr->type = hole;
    list->num--;
    
}

void compact(linkedList *list){ 
    // TODO: combine adjacent holes
    linkedList *ptr=list, *next;
    linkedList *hole_start_ptr = NULL;
    int hole_count=0;
    while (ptr != NULL){
        next = ptr->next;
        if(ptr->type==hole){
            if (hole_count==0){ // save ptr to hole
                hole_start_ptr = ptr;
            }
            else{ // consume the data - and free the block
                hole_start_ptr->next = ptr->next; // set hole start next to the current next
                hole_start_ptr->size += ptr->size; // increase hole start by current size
                free(ptr); // free the memory at this pointer
            }
            hole_count++;
        } else{ // process
            hole_count = 0; // reset hole counter
        }
        ptr = next;
    }
}


void fill(linkedList *list, int* byte_counter){
    srand(1);
    int fill_size;
    while(*byte_counter < MAX_BYTES){
        fill_size = (rand()% MAX_BLOCK_FACTOR + 1) * MIN_BLOCK;
        printf("trying to fill with size %d\n", fill_size);
        int res = allocate(fill_size, list, first_fit, byte_counter);
        if (res) {
            *byte_counter += fill_size;
        }
    }
}

/*void fill_alt(linkedList *list) {
    srand(1);
    int val = 1;
    do {
        val = (rand() % 256) * 4;
    } while (allocate(val, list,  first_fit)); 
}*/

void traverse_remove(linkedList *list, int reduce, int* byte_counter){
    linkedList* ptr = list;
    int remove = round(ptr->num / reduce); // number of processes to remove
    int i = 0;
    while (i<remove){
        int crawl = rand() % 24;
        int j = 0;
        while (1){ // get to random process
            //printf("%d\n", ptr->start);
            if (ptr->type == process){
                j++; // increment processes only
                if (j == crawl){
                    break;
                }
            }
            ptr = ptr->next;
            if (ptr == NULL){
                ptr = list;
            }
        }
        release(ptr->start, ptr, byte_counter); // remove process
        printf("removed process at %d with size %d\n", ptr->start, ptr->size);
        *byte_counter -= ptr->size;
        i++;
    }
}

int main(int argc, char ** argv){
    
    int byte_counter = 0;

    // int *memory_region = calloc(MAX_BYTES, sizeof(int)); // don't actually need to allocate memory - just use the maintainer
    linkedList maintainer = {
        .type=hole,
        .start=0,
        .size=MAX_BYTES,
        .num=0,
        .next=NULL
    };

    srand(1); // seed

    /*allocate(8, &maintainer, first_fit);
    allocate(1024, &maintainer, first_fit);
    allocate(16, &maintainer, first_fit);
    status(&maintainer);
    release(0, &maintainer);
    release(8, &maintainer);
    release(1032, &maintainer);*/
    
    // part b)
    fill(&maintainer, &byte_counter);
    printf("status after filling: \n\n");
    status(&maintainer, &byte_counter);


    // part c)
    int reduction = 2;
    traverse_remove(&maintainer, reduction, &byte_counter);
    printf("status after removing %d percent: \n\n", (100/reduction));
    status(&maintainer, &byte_counter);

    // fill memory
    // int bytes_filled = 0;
    // int fill_size;
    // while(bytes_filled < MAX_BYTES){
    //     fill_size = (rand()%MAX_BLOCK_FACTOR + 1) * MIN_BLOCK; 
    //     int res = allocate(fill_size, &maintainer, first_fit);
    //     if (res) {
    //         bytes_filled+=fill_size;
    //     }
    // }
    
    // part e) DO THIS WITH EACH MODE TO TEST UTILIZATION
    fill(&maintainer, &byte_counter);
    printf("status after filling (using first fit): \n\n");
    status(&maintainer, &byte_counter);
    
    // part f): traverse & compact
    /*
    traverse_remove(&maintainer, reduction, &byte_counter);
    printf("status after removing %d percent: \n\n", (100/reduction));
    status(&maintainer, &byte_counter);
    compact(&maintainer);
    printf("status after compacting: \n\n");
    status(&maintainer, &byte_counter);*/

    return 0;
}

// EVALUATION METRICS:
/*
- number of holes at completion
- amount of unused memory at completion
- average size of hole & number of holes after each allocation 
*/