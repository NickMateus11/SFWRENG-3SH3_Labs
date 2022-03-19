#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_BYTES           1048576 // 1MB  
#define MIN_BLOCK           4096    // 4KB 
#define MAX_BLOCK_FACTOR    25      // 25 * 4096 = 100KB

#define SEED                1

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

void compact_adj(linkedList *list){ 
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

void compact(linkedList *list){
    if (list == NULL) return;

    // compact adjacent holes first - ensure hole run length is 1
    compact_adj(list);

    linkedList *ptr=list, *prev_ptr, *next_node;
    int hole_size = 0;

    // special case: first element is a hole
    if (ptr != NULL && ptr->type==hole && ptr->next != NULL){
        hole_size += ptr->size; //update hole size

        linkedList *next_next = ptr->next->next;
        ptr->type = ptr->next->type; // copy fields
        ptr->size = ptr->next->size;
        free(ptr->next); // free ptr        
        ptr->next = next_next;
    }

    prev_ptr = ptr;
    ptr = ptr->next;

    while(ptr != NULL){
        next_node = ptr->next;
        if (ptr->type == hole){
            hole_size += ptr->size; // save hole size
            prev_ptr->next = next_node; // change prev's next to current next (skipping this node)
            free(ptr); // free the memory
        } else{ // node is a process
            // shift start address by current hole size count
            ptr->start -= hole_size; 

            // only update prev_ptr if current node is a process
            prev_ptr = ptr;
        }
        ptr = next_node;
    }

    // reached end of list
    // create large contiguous hole
    if (hole_size > 0){
        prev_ptr->next = (linkedList *)malloc(sizeof(linkedList));
        prev_ptr->next->type  = hole;
        prev_ptr->next->size  = hole_size;
        prev_ptr->next->start = prev_ptr->start + prev_ptr->size;
        prev_ptr->next->next  = NULL;
    }
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
            // printf("NO Available spot found\n");
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
            // printf("NO Available spot found\n");
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
            // printf("NO Available spot found\n");
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

    // "release"
    ptr->type = hole;
    compact_adj(list);
}

void status(linkedList *list){
    linkedList *ptr = list;
    while(ptr != NULL){
        // printf("TYPE:%d,\t\tSTART:%d,\tSIZE:%d\n",ptr->type, ptr->start, ptr->size);
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

int num_holes(linkedList *list){
    int holes = 0;
    linkedList *ptr = list;
    while(ptr != NULL){
        if (ptr->type == hole) holes += 1;
        ptr = ptr->next;
    }
    return holes;
}

void fill_all(linkedList *list){
    // generate new seed
    // time_t t;
    // srand(time(&t));
    srand(SEED+1);

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
    srand(SEED+2);
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
    srand(SEED+3);
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

float avg_hole_size(linkedList *list){
    int n = num_holes(list);
    if (n==0) return 0;

    float avg = 0;
    linkedList *ptr=list;
    while(ptr != NULL){
        if (ptr->type == hole){
            avg += ptr->size;
        }
        ptr =ptr->next;
    }
    return avg/n;
}

int main(int argc, char ** argv){

    // // part a)
    // int *memory_region = calloc(MAX_BYTES, sizeof(int)); // don't actually need to allocate memory - just use the maintainer
    
    linkedList default_maintainer = {
        .type=hole,
        .start=0,
        .size=MAX_BYTES,
        .next=NULL
    };

/*********************************************/
// NO COMPACTION

    printf("\n\n***** WITHOUT COMPACTION *****\n\n");

    linkedList  maintainer_first_fit = default_maintainer, 
                maintainer_best_fit  = default_maintainer, 
                maintainer_worst_fit = default_maintainer; 

    // // part b)
    fill_all(&maintainer_first_fit);
    fill_all(&maintainer_best_fit);
    fill_all(&maintainer_worst_fit);
    printf("status after filling:\n");
    status(&maintainer_first_fit);

    // // part c)
    float remove_factor = 0.5f;
    int num_to_release = (int)(blocks_occupied(&maintainer_first_fit)*remove_factor);
    random_release(&maintainer_first_fit, num_to_release);
    random_release(&maintainer_best_fit, num_to_release);
    random_release(&maintainer_worst_fit, num_to_release);
    printf("status after random release:\n");

    // // part d)
    status(&maintainer_first_fit);

    // // part e)
    printf("FIRST FIT:\n");
    printf("status after re-fill:\n");
    fill_until_first_failure(&maintainer_first_fit, first_fit);
    printf("average hole size: %0.2f\n", avg_hole_size(&maintainer_first_fit));
    printf("Num holes: %d\n", num_holes(&maintainer_first_fit));
    status(&maintainer_first_fit);

    printf("BEST FIT:\n");
    printf("status after re-fill:\n");
    fill_until_first_failure(&maintainer_best_fit, best_fit);
    printf("average hole size: %0.2f\n", avg_hole_size(&maintainer_best_fit));
    printf("Num holes: %d\n", num_holes(&maintainer_best_fit));
    status(&maintainer_best_fit);

    printf("WORST FIT:\n");
    printf("status after re-fill:\n");
    fill_until_first_failure(&maintainer_worst_fit, worst_fit);
    printf("average hole size: %0.2f\n", avg_hole_size(&maintainer_worst_fit));
    printf("Num holes: %d\n", num_holes(&maintainer_worst_fit));
    status(&maintainer_worst_fit);

/*********************************************/
// COMPACTION

    printf("\n\n***** WITH COMPACTION *****\n\n");

    linkedList  maintainer_first_fit_compact = default_maintainer, 
                maintainer_best_fit_compact  = default_maintainer, 
                maintainer_worst_fit_compact = default_maintainer; 

    // // part b)
    fill_all(&maintainer_first_fit_compact);
    fill_all(&maintainer_best_fit_compact);
    fill_all(&maintainer_worst_fit_compact);
    printf("status after filling:\n");
    status(&maintainer_first_fit_compact);

    // // part c)
    remove_factor = 0.5f;
    num_to_release = (int)(blocks_occupied(&maintainer_first_fit_compact)*remove_factor);
    random_release(&maintainer_first_fit_compact, num_to_release);
    random_release(&maintainer_best_fit_compact, num_to_release);
    random_release(&maintainer_worst_fit_compact, num_to_release);
    printf("status after random release:\n");

    // // part d)
    status(&maintainer_first_fit_compact);

    // compaction
    compact(&maintainer_first_fit_compact);
    compact(&maintainer_best_fit_compact);
    compact(&maintainer_worst_fit_compact);
    printf("status after compacting:\n");
    status(&maintainer_first_fit_compact);

    // re-fill until failure
    printf("FIRST FIT:\n");
    printf("status after re-fill:\n");
    fill_until_first_failure(&maintainer_first_fit_compact, first_fit);
    printf("average hole size: %0.2f\n", avg_hole_size(&maintainer_first_fit_compact));
    printf("Num holes: %d\n", num_holes(&maintainer_first_fit_compact));
    status(&maintainer_first_fit_compact);

    printf("BEST FIT:\n");
    printf("status after re-fill:\n");
    fill_until_first_failure(&maintainer_best_fit_compact, best_fit);
    printf("average hole size: %0.2f\n", avg_hole_size(&maintainer_best_fit_compact));
    printf("Num holes: %d\n", num_holes(&maintainer_best_fit_compact));
    status(&maintainer_best_fit_compact);

    printf("WORST FIT:\n");
    printf("status after re-fill:\n");
    fill_until_first_failure(&maintainer_worst_fit_compact, worst_fit);
    printf("average hole size: %0.2f\n", avg_hole_size(&maintainer_worst_fit_compact));
    printf("Num holes: %d\n", num_holes(&maintainer_worst_fit_compact));
    status(&maintainer_worst_fit_compact);

    return 0;
}
