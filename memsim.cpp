#include <stdio.h>
#include <iostream>
#include <fstream>
#include <deque>    
#include <vector>
#include <map>
#include <string>
#include <sstream>


void fifo(char* tracefile, int nframes, std::string dq){
    //initialize necessary variables.   
    int memFrames = nframes;
    int eventtr = 0;
    int diskread = 0;
    int diskwrite = 0;
    FILE *mfile = fopen(tracefile, "r");

    unsigned addr; char rw;

    //read trace file
    while(fscanf(mfile, "%x %c", &addr, &rw) != EOF){
        std::cout<<addr<<" "<<rw <<"\n";
    }

    fclose(mfile);
}

void lru(std::string tracefile, int nframes, std::string dq){

}

void segmentedfifo(std::string tracefile, int nframes, int p, std::string dq)
{

}


int main(int argc, char **argv)
{
    //get inputs from argv
    char* tracefilename;
    int nframes;
    std::string algorithm;
    std::string dq;
    int p = -1;


    tracefilename = argv[1];
    nframes = atoi(argv[2]);
    algorithm = argv[3];
    if(argc == 5){
        dq = argv[4];   

    }
    else if(argc == 6){
        p = std::stoi(argv[4]);
        dq = argv[5];
    }
    else{ //bad input
        std::cout<<"Invalid input";
    }

    //what algorithm to use?
    if(algorithm.compare("fifo") == 0 && p == -1){
        fifo(tracefilename, nframes, dq);
    }
    else if(algorithm.compare("lru") == 0 && p == -1){
        lru(tracefilename, nframes, dq);
    }
    else if(algorithm.compare("vms") == 0 && p != -1){
        segmentedfifo(tracefilename, nframes, p, dq);
    }

}