#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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
        linkedList *newHole = (linkedList *) malloc(sizeof(linkedList));
        newHole->type  = hole;
        newHole->size  = list_item->size - size;
        newHole->start = list_item->start + size;
        newHole->next  = list_item->next;

        list_item->next = newHole;
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

void status(linkedList *list){
    linkedList *ptr = list;
    while(ptr != NULL){
        printf("TYPE:%d,\t\tSTART:%d,\tSIZE:%d\n",ptr->type, ptr->start, ptr->size);
        ptr = ptr->next;
    }
    printf("\n");
}

int bytes_occupied(linkedList *list){
    int byte_count = 0;
    linkedList *ptr = list;
    while(ptr != NULL){
        if (ptr->type == process) byte_count += ptr->size;
        ptr = ptr->next;
    }
    return byte_count;
}

int blocks_occupied(linkedList *list){
    int process_count = 0;
    linkedList *ptr = list;
    while(ptr != NULL){
        if (ptr->type == process) process_count += 1;
        ptr = ptr->next;
    }
    return process_count;
}

void fill_all(linkedList *list){
    // generate new seed
    time_t t;
    srand(time(&t));
    
    int bytes_filled = bytes_occupied(list);
    int fill_size;
    while(bytes_filled < MAX_BYTES){
        fill_size = (rand()%MAX_BLOCK_FACTOR + 1) * MIN_BLOCK; 
        int res = allocate(fill_size, list, first_fit);
        if (res) {
            bytes_filled+=fill_size;
        }
    }
}

void fill_until_first_failure(linkedList *list, allocation_mode mode) {
    srand(1);
    int fill_size = (rand()%MAX_BLOCK_FACTOR + 1) * MIN_BLOCK;
    int count = 0;
    while (allocate(fill_size, list, mode)){
        fill_size = (rand()%MAX_BLOCK_FACTOR + 1) * MIN_BLOCK;
        count++;
    }
    // print count - reminder: new holes are created when allocating,
    // so count may be larger than the number of holes available when
    // this function is first called
    printf("Filled - %d - holes\n", count);
}

void random_release(linkedList *list, int release_count){
    srand(1);
    linkedList* ptr = list;
    int num_blocks_occupied = blocks_occupied(list);
    int count = 0;
    while(count != release_count){
        // select random block to remove
        int block_to_remove = rand()%num_blocks_occupied;
        int i=0;
        // locate block
        while(1){
            if (ptr->type == process) {
                if (i==block_to_remove){
                    // release block
                    release(ptr->start, list);
                    break;
                }
                i++;
            }
            ptr = ptr->next;               
        }
        // update values
        count++;
        ptr = list;
        num_blocks_occupied--;
    }
    printf("Removed - %d - processes\n", release_count);
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

    // // part b)
    fill_all(&maintainer);
    printf("status after filling:\n");
    status(&maintainer);

    // // part c)
    float remove_factor = 0.5f;
    int num_to_release = (int)(blocks_occupied(&maintainer)*remove_factor);
    random_release(&maintainer, num_to_release);
    printf("status after random release:\n");
    status(&maintainer);

    // re-fill until failure
    fill_until_first_failure(&maintainer, first_fit);
    printf("status after re-fill:\n");
    status(&maintainer);

    compact(&maintainer);
    printf("status after compacting: \n\n");
    status(&maintainer);

    return 0;
}
