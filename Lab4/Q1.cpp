#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>


void print_results(const std::vector<int> &ref_string, const std::vector<std::vector<int>> &state, const std::vector<bool> &faults){

    // print ref string
    std::cout << "  ";
    for (int i=0; i<ref_string.size(); i++){
        std::cout << ref_string[i] << "  ";
    }
    std::cout << std::endl;

    // print formatting line ----
    for(int i=0; i< ref_string.size()*3; i++){
        std::cout << "-";
    }
    std::cout << std::endl;
    
    // print frame states
    for (int i=0; i<state[i].size(); i++){
        std::cout << " ";
        for (int j=0; j<state.size(); j++){
            if (state[j][i] != -1) std::cout << " ";
            std::cout << state[j][i] << " ";
        }
        std::cout << std::endl;
    }
    
    // print page-faults
    std::cout << "  ";
    int fault_count = 0;
    for(int i=0; i<faults.size(); i++){
        std::cout << (faults[i]? "p  ":"   ");
        if (faults[i]) fault_count++;
    }
    std::cout << std::endl;

    // print formatting line ----
    for(int i=0; i< ref_string.size()*3; i++){
        std::cout << "-";
    }
    std::cout << std::endl;

    // print page fault count
    std::cout << fault_count << " page-faults" << std::endl << std::endl;

    return;
}

void FIFO(const std::vector<int> &page_refs, int num_frames){
    std::cout << "FIFO" << std::endl;

    // store all states for printing later
    std::vector<std::vector<int>> frame_state_all;
    std::vector<bool> page_faults(page_refs.size(), false);
    
    // loop variables
    int page_fault_count = 0;
    std::vector<int> frame_state_current(num_frames, -1);    
    
    for (int i=0; i< page_refs.size(); i++){
        bool page_fault_occured = true;
        for (int j=0; j<num_frames; j++){
            if (frame_state_current[j] == page_refs[i]){
                // page already loaded in a frame
                page_fault_occured = false;
                break; 
            }
        }
        if (page_fault_occured){
            // page fault
            page_fault_count++;
            page_faults[i] = true;
            // insert page into a frame - FIFO style
            for (int j=frame_state_current.size(); j>0; j--){
                // shift things down - this is why its called FIFO
                frame_state_current[j] = frame_state_current[j-1];
            }
            frame_state_current[0] = page_refs[i];
        }

        // save frame_state
        frame_state_all.push_back(frame_state_current);
    }

    print_results(page_refs, frame_state_all, page_faults);

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
    std::cout << std::endl << std::endl;

    // Page Replacement Algorithms

    // FIFO
    FIFO(page_refs, M);

    // Optimal
    optimal(page_refs, M);

    // LRU
    LRU(page_refs, M);
    
    return 0;
}