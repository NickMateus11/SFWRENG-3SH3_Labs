#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>


void FIFO(std::vector<int> page_refs){
    return;
}
void optimal(std::vector<int> page_refs){
    return;
}
void LRU(std::vector<int> page_refs){
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
    FIFO(page_refs);

    // Optimal
    optimal(page_refs);

    // LRU
    LRU(page_refs);
    
    return 0;
}