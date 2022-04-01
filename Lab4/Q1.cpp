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
    std::cout << "Optimal" << std::endl;
    
    // store all states for printing later
    std::vector<std::vector<int>> frame_state_all;
    std::vector<bool> page_faults(page_refs.size(), false);
    
    // loop variables
    int page_fault_count = 0;
    std::vector<int> frame_state_current(num_frames, -1);  
    
    // optimal algorithm variables
    int wait_time = 0;
    int wait_page = 0;
    
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
            
            // for initial page_faults just push pages into available frames
            if (page_fault_count <= num_frames)
                frame_state_current[page_fault_count - 1] = page_refs[i];
                
            // otherwise perform optimal algorithm
            else
            {
                // reset variables to track longest page wait
                wait_time = 0; 
                wait_page = 0;
                
                // parse thru each frame to see its wait time
                for (int j=0; j < frame_state_current.size(); j++)
                {
                    // start at current page and count next time we see each frame
                    for (int k=i; k < page_refs.size(); k++)
                    {
                        // found page in reference string corresponding to frame
                        if (frame_state_current[j] == page_refs[k])
                        {
 
                            // this frame has a longer wait, new optimal replacement
                            if (k - i > wait_time)
                            {
                                wait_time = k - i;
                                wait_page = j;
                            }
                            
                            break;
                        }
                        
                        // page isn't used again, replace it
                        else if (k == page_refs.size() - 1)
                        {
                            wait_time = 9999; // set very high so it can't be overwritten
                            wait_page = j;
                        }
                    }
                }
                
                // replace optimal frame with new page
                frame_state_current[wait_page] = page_refs[i];
            }
        }

        // save frame_state
        frame_state_all.push_back(frame_state_current);
    }

    print_results(page_refs, frame_state_all, page_faults);

    return;
}

void printMatrix(std::vector<std::vector<int>> mat, int x, int y){
    for(int i = 0; i < x; i++) {
        for(int j = 0; j < y; j++) {
            std::cout << mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int checkLowest(std::vector<std::vector<int>> mat, int x, int y){

    int sum = 0;
    int lru = 0;
    int val = y+1;

    for (int i=0; i<y; i++){ // row
        sum = 0;
        for (int j=0; j<x; j++){ // column
            sum += mat[i][j];
            //std::cout << "row " << i << " = " << sum << std::endl;
        }
        if (sum < val){ // check if the current row has been used less than the current least
            lru = i; // set lru to updated least recently used
            val = sum;
        }
    }
    // std::cout << "lru = " << lru << std::endl; 
    return lru;
}

int find(std::vector<int> arr, int key) {
    for(int i=0; i<arr.size(); i++){
        if(arr[i] == key) {
            return i;
        }
    }
    return -1;
}

void LRU(std::vector<int> page_refs, int num_frames){
    std::cout << "LRU" << std::endl;

    std::vector<std::vector<int>> bin_matrix(num_frames, std::vector<int> (num_frames, 0)); // 2D binary matrix to keep track of usages
        
    int k = 0; // used to store current page ref
    std::vector<bool> page_faults(page_refs.size(), false); // vector to keep track of page faults
    int page_fault_count = 0; // fault counter
    std::vector<int> frame_state_current(num_frames, -1); // current frame state
    int current = 0; // frame to be used next
    std::vector<std::vector<int>> frame_state_all; // all frame states over entire process

    for (int i=0; i<page_refs.size(); i++){
        k = page_refs[i];
        
        //printMatrix(bin_matrix, num_frames, num_frames);
        current = checkLowest(bin_matrix, num_frames, num_frames); // compute the LRU frame
        // std::cout << "least recently used frame: " << current << std::endl;
        
        // place page into frame if not currently in a frame
        // if the page is already in use, continue to next iteration
        auto idx = find(frame_state_current, k);
        // std::cout << "idx = " << idx << std::endl;
        if (idx != -1) {
            // std::cout << "replacement at " <<  idx << std::endl;
            frame_state_current[idx] = k;
            current = idx;
        }
        else {
            // std::cout << "lru at " << current << std::endl;
            frame_state_current[current] = k;
            page_faults[i] = true;
            page_fault_count++;
        }
        
        //std::cout << "k = " << k << std::endl;
        for (int j = 0; j<num_frames; j++){ // set every element of row k to 1
            //std::cout << "attempting to insert 1 at (" << j << ", " << current << ")" << std::endl;
            bin_matrix[current][j] = 1; // dies here???
        }
        
        for (int j = 0; j<num_frames; j++){ // set every element of column k to 0
            //std::cout << "attempting to insert 0 at (" << j << ", " << current << ")" << std::endl;
            bin_matrix[j][current] = 0;
        }
        
        
        frame_state_all.push_back(frame_state_current); // save frame state
        // std::cout << std::endl << "***************************************" << std::endl;
    }

    print_results(page_refs, frame_state_all, page_faults);
    
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
