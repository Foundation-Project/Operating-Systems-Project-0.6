#include <stdio.h>
#include <iostream>
#include <fstream>
#include <deque>    
#include <vector>
#include <string>

int main(int argc, char **argv)
{
    //get inputs from argv
    std::string tracefilename;
    int nframes;
    std::string algorithm;
    int dq;
    int p = -1;

    tracefilename = argv[2];
    nframes = argv[3];
    algorithm = argv[4];

    if(argv == 5){
        dq = argv[5];   

    }
    else if(argv == 6){
        p = argv[5];
        dq = argv[6];
    }
    else{
        std::cout<<"Invalid input";
    }

    //what algorithm to use?
    if(algorithm.compare("fifo") == 0 && p == -1){
        fifo(tracefilename, nframes, dq)
    }
    else if(algorithm.compare("lru") == 0 && p == -1){
        lru(tracefilename, nframes, dq)
    }
    else if(algorithm.compare("vms") == 0 && p != -1){
        segmentedfifo(tracefilename, nframes, p, dq)
    }

}

void fifo(std::string tracefile, int nframes, int dq){

}

void lru(std::string tracefile, int nframes, int dq){

}

void segmentedfifo(std::string tracefile, int nframes, int p, int debug)
{

}