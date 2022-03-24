#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>


void FIFO(const std::vector<int> &page_refs, int num_frames){
    std::cout << "FIFO" << std::endl;

    int page_fault_count = 0;
    std::vector<int> frame_state(num_frames, -1);
    
    for (int i=0; i< page_refs.size(); i++){
        bool page_fault_occured = true;
        for (int j=0; j<num_frames; j++){
            if (frame_state[j] == page_refs[i]){
                // page already loaded in a frame
                page_fault_occured = false;
                break; 
            }
        }
        if (page_fault_occured){
            // page fault
            page_fault_count++;
            // insert page into a frame - FIFO style
            for (int j=frame_state.size(); j>0; j--){
                //shift
                frame_state[j] = frame_state[j-1];
            }
            frame_state[0] = page_refs[i];
        }
    
        // print result after this iteration
        std::cout << " " << page_refs[i] << " | ";
        for (int j=0; j<frame_state.size(); j++){
            std::cout << frame_state[j] << " ";
        }
        if (page_fault_occured){
            std::cout << "\tp";
        }
        std::cout << std::endl;

    }

    std::cout << page_fault_count << " page-faults" << std::endl;

    return;
}
void optimal(std::vector<int> page_refs, int num_frames){
    return;
}
void LRU(std::vector<int> page_refs, int num_frames){
    return;
}

int main(int argc, char **argv){
    
    // get filename
    char filename[32];
    if (argc == 2){
        strcpy(filename, argv[1]);
    } else {    
        strcpy(filename, "sample.dat");
    }

    // parse file
    FILE *p_file = fopen(filename, "r");
    if (errno != 0){
        perror("File Error");
        exit(1);
    }
    
    int N, M;
    std::vector<int> page_refs;
    
    fscanf(p_file, "%d %d", &N, &M);
    
    int n;
    while(1){
        fscanf(p_file, "%d", &n);
        if (n!=-1){
            page_refs.push_back(n);
        } else break;
    }
    fclose(p_file);

    // print file contents
    std::cout << "Number of pages: " << N << std::endl;
    std::cout << "Number of frames: " << M << std::endl;
    std::cout << "Size of the reference String: " << page_refs.size() << std::endl;
    std::cout << "Reference String:" << std::endl;
    for (int i=0; i<page_refs.size(); i++){
        std::cout << " " << page_refs[i];
    }
    std::cout << std::endl;

    // Page Replacement Algorithms

    // FIFO
    FIFO(page_refs, M);

    // Optimal
    optimal(page_refs, M);

    // LRU
    LRU(page_refs, M);
    
    return 0;
}