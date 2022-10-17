#include <stdio.h>
#include <iostream>
#include <fstream>
#include <deque>    
#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <sstream>


void fifo(char* tracefile, int nframes, std::string dq){
    //initialize necessary variables.
    std::vector<std::pair<int, char>> mem;  
    std::vector<std::pair<int, char>>::iterator itr1;
    std::vector<std::pair<int, char>>::iterator itr2 = mem.begin();

    int memFrames = nframes;
    int eventtr = 0;
    int diskread = 0;
    int diskwrite = 0;
    FILE *mfile = fopen(tracefile, "r");

    unsigned addr; char rw;

    //read trace file
    while(fscanf(mfile, "%x %c", &addr, &rw) != EOF){
        eventtr +=1;
        //when reading memory addresses from file, divide by 4096 or left bit shift by 12, since page size is 4 KB
        addr /= 4096;

        for(itr1 = mem.begin(); itr1 < mem.end(); itr1++)    //check if in page table
        {
            if(itr1->first == addr)
                break;
        }

        if(itr1 != mem.end()){ //is present in fifo
            switch(itr1->second){
                case 'R':
                    itr1->second = rw;
                break;
            }

        }
        else{ //is not present in fifo
            if(mem.size() < memFrames) //fifo not full
            {
                mem.push_back(std::pair<int,char>(addr, rw));
            } 
            else{   //fifo is full
                if(mem.begin()->second == 'W')
                {
                    diskwrite++;
                }
                mem.erase(mem.begin());
                mem.push_back(std::pair<int,char>(addr, rw));
            }

            diskread++;
        }



    }

    fclose(mfile);
    std::cout<<"total memory frames: "<<memFrames<<std::endl;
    std::cout<<"events in trace: "<<eventtr<<std::endl;
    std::cout<<"total disk reads: "<<diskread<<std::endl;
    std::cout<<"total disk writes: "<<diskwrite<<std::endl;
}

void lru(char* tracefile, int nframes, std::string dq){
    //initialize necessary variables.
    std::vector<std::pair<int, char>> mem;  
    std::vector<std::pair<int, char>>::iterator itr1;
    std::vector<std::pair<int, char>>::iterator itr2 = mem.begin();

    int memFrames = nframes;
    int eventtr = 0;
    int diskread = 0;
    int diskwrite = 0;
    FILE *mfile = fopen(tracefile, "r");

    unsigned addr; char rw;

    //read trace file
    while(fscanf(mfile, "%x %c", &addr, &rw) != EOF) {
        addr /= 4096;
        eventtr++;

        for(itr1 = mem.begin(); itr1 < mem.end(); itr1++)    //check if in page table
        {
            if(itr1->first == addr)
                break;
        }

        if(itr1 != mem.end()){ //is present in lru
            char origchar = itr1->second;
            mem.erase(itr1);    //delete
            if(origchar == 'W' && rw == 'R'){
                rw = 'W';

            }
            mem.push_back(std::pair<int, char>(addr, rw));       
        } 
        else{ //is not present in fifo
            if(mem.size() < memFrames) //page is not in lru and lru is not full
            {
                mem.push_back(std::pair<int, char>(addr, rw)); //add new page to the top of LRU
            } 
            else{   //page is not in lru and lru is full
                if(mem.begin()->second == 'W'){ diskwrite++;}
                mem.erase(mem.begin()); //eject oldest page
                mem.push_back(std::pair<int, char>(addr, rw)); //add new page to the top

            }
            diskread++;
        }
    }
    fclose(mfile);
    std::cout<<"total memory frames: "<<memFrames<<std::endl;
    std::cout<<"events in trace: "<<eventtr<<std::endl;
    std::cout<<"total disk reads: "<<diskread<<std::endl;
    std::cout<<"total disk writes: "<<diskwrite<<std::endl;    

}

void segmentedfifo(char* tracefile, int nframes, int p, std::string dq)
{
    //initialize the variables
    int memFrames = nframes;
    int eventtr = 0;
    int diskread = 0;
    int diskwrite = 0;

    std::vector<std::pair<int, char>> mem;  
    std::vector<std::pair<int, char>>::iterator itr1;

    unsigned addr; char rw;
    FILE *mfile = fopen(tracefile, "r");

    

    //read trace file
    while(fscanf(mfile, "%x %c", &addr, &rw) != EOF){
        eventtr++;

    }

    fclose(mfile);
    std::cout<<"total memory frames: "<<memFrames<<std::endl;
    std::cout<<"events in trace: "<<eventtr<<std::endl;
    std::cout<<"total disk reads: "<<diskread<<std::endl;
    std::cout<<"total disk writes: "<<diskwrite<<std::endl;


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