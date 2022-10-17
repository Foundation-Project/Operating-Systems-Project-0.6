#include <stdio.h>
#include <iostream>
#include <fstream>
#include <deque>    
#include <queue>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <sstream>


void fifo(char* tracefile, int nframes, std::string dq){
    //initialize necessary variables.
    std::vector<std::pair<unsigned, char> > mem;  
    std::vector<std::pair<unsigned, char> >::iterator itr1;
    std::vector<std::pair<unsigned, char> >::iterator itr2 = mem.begin();

    unsigned int memFrames = nframes;
    int eventtr = 0;
    int diskread = 0;
    int diskwrite = 0;
    bool debugger;
    FILE *mfile = fopen(tracefile, "r");

    unsigned addr; char rw;

    //check if in debug or quiet
    if(dq.compare("debug") == 0){debugger = true;}
    else{debugger = false;}

    /****** DEBUG VARIABLES ******/
    int numhits = 0;
    int nummiss = 0;

    //read trace file
    while(fscanf(mfile, "%x %c", &addr, &rw) != EOF){
        eventtr +=1;
        //when reading memory addresses from file, divide by 4096 or left bit shift by 12, as page size is 4 KB
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
            numhits++;

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
            nummiss++;
        }



    }

    fclose(mfile);
    std::cout<<"total memory frames: "<<memFrames<<std::endl;
    std::cout<<"events in trace: "<<eventtr<<std::endl;
    std::cout<<"total disk reads: "<<diskread<<std::endl;
    std::cout<<"total disk writes: "<<diskwrite<<std::endl;

    /****** DEBUG OUTPUT ******/
    if(debugger == true){
        std::cout<<"number of hits: "<<numhits<<std::endl;
        std::cout<<"number of misses: "<<nummiss<<std::endl;
    }
}

void lru(char* tracefile, int nframes, std::string dq){  
   //initialize necessary variables.
    std::vector<std::pair<unsigned, char> > mem;  
    std::vector<std::pair<unsigned, char> >::iterator itr1;
    std::vector<std::pair<unsigned, char> >::iterator itr2 = mem.begin();

    unsigned int memFrames = nframes;
    int eventtr = 0;
    int diskread = 0;
    int diskwrite = 0;
    bool debugger;
    FILE *mfile = fopen(tracefile, "r");

    unsigned addr; char rw;

        //check if in debug or quiet
    if(dq.compare("debug") == 0){debugger = true;}
    else{debugger = false;}

    /****** DEBUG VARIABLES ******/
    int numhits = 0;
    int nummiss = 0;

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
            numhits++;      
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
            nummiss++;
        }
    }
    fclose(mfile);
    std::cout<<"total memory frames: "<<memFrames<<std::endl;
    std::cout<<"events in trace: "<<eventtr<<std::endl;
    std::cout<<"total disk reads: "<<diskread<<std::endl;
    std::cout<<"total disk writes: "<<diskwrite<<std::endl;  

        /****** DEBUG OUTPUT ******/
    if(debugger == true){
        std::cout<<"number of hits: "<<numhits<<std::endl;
        std::cout<<"number of misses: "<<nummiss<<std::endl;
    }  

}

void vms(char* tracefile, int nframes, int p, std::string dq)
{

    std::vector <unsigned> frame;
    std::vector <char> read_write;
    std::deque <unsigned> lruVector;
    std::queue <unsigned> order;
    std::set <unsigned> set;
    std::map <unsigned, unsigned> memory;

    unsigned addr; char rw;
    int diskread = 0;
    int diskwrite = 0;
    
    //read the file
    FILE *mfile = fopen(tracefile, "r");
    
    while (fscanf(mfile,"%x %c",&addr,&rw) != EOF) {
        frame.push_back(addr);
        read_write.push_back(rw);   
    }

    //frame check
    long unsigned int lruFrame = 0, fifoFrame = 0;
    lruFrame = (p * nframes) / 100;
    fifoFrame = nframes - lruFrame;
    
    
    if (fifoFrame == 0)  {
        lru(tracefile, nframes, dq);
        return;
    }
    
    else if (lruFrame == 0) 
    {
        fifo(tracefile, nframes, dq);
        return;
    }

    //trace check
    for (unsigned int i = 0; i < frame.size(); i++) {
        if (set.end() == set.find(frame[i]/4096) && fifoFrame == set.size()) {
            if (find(lruVector.begin(), lruVector.end(), frame[i]/4096) == lruVector.end() && lruFrame == lruVector.size())
            {   //adding 
                if (read_write[i] == 'W'){
                    memory[frame[i]/4096] = 1;
                }
                if (memory[lruVector.front()] == 1)
                {
                    diskwrite++;
                    memory[lruVector.front()] = 0;
                }
                lruVector.pop_front();

                lruVector.push_back(order.front());
                set.erase(order.front());
                order.pop(); 
                order.push(frame[i]/4096);
                set.insert(frame[i]/4096);
                diskread++;
            } //adding
            else if (find(lruVector.begin(), lruVector.end(), frame[i]/4096) == lruVector.end() && lruFrame > lruVector.size())
            {
                lruVector.push_back(order.front());

                set.erase(order.front());
                order.pop();

                order.push(frame[i]/4096);
                set.insert(frame[i]/4096);

                if (read_write[i] == 'W')
                    memory[frame[i]/4096] = 1;
                diskread++;
            } //adding
            else if (!(find(lruVector.begin(), lruVector.end(), frame[i]/4096) == lruVector.end()))
            {
                
                std::deque<unsigned>::iterator vms = find(lruVector.begin(), lruVector.end(), frame[i]/4096);
                order.push(*vms);
                set.insert(*vms);

                lruVector.erase(vms);
                lruVector.push_back(order.front());

                set.erase(order.front());
                order.pop();

                if (read_write[i] == 'W')
                    memory[frame[i]/4096] = 1;
            }
        } //adding
        else if(set.size() < fifoFrame && set.find(frame[i]/4096) == set.end()) {
            order.push(frame[i]/4096);
            set.insert(frame[i]/4096);
            if (read_write[i] == 'W')
                memory[frame[i]/4096] = 1;
            diskread++;
        }
        else
        {
            if(read_write[i] == 'W'){
                memory[frame[i]/4096] = 1;
            }
        }

    }
    fclose(mfile); //remember to close the file!

    //results:
    std::cout << "Memory Frames: " << nframes << std::endl;
    std::cout << "Trace Count: " << frame.size() << std::endl; 
    std::cout << "Read Count: " << diskread << std::endl;
    std::cout << "Write Count: " << diskwrite << std::endl;


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
    nframes = std::atoi(argv[2]);
    algorithm = argv[3];
    if(argc == 5){
        dq = argv[4];   

    }
    else if(argc == 6){
        p = std::atoi(argv[4]);
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
        vms(tracefilename, nframes, p, dq);
    }
}