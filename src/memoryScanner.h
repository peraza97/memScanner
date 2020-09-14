#ifndef __MEMORY_SCANNER_H__
#define __MEMORY_SCANNER_H__

#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>    
#include <cstring>
#include <vector>
#include <iostream>
#include <inttypes.h>


#include "util.h"

using namespace std;

class MemoryScanner{
    private:
        //scanner variables
        pid_t pid;
        container_t * container;
        datatype dataType;
        string numBytesStr;
        uint64_t numBtyes;
        address_t startAddress;
        address_t endAddress;
        bool running;

        //target variables 
        int targetInt;
        char targetChar;
        string targetString;

        //helper functions
        uint64_t convertSizeInput(string);
        datatype convertToDataType(int);
        address_t getContainerIndex();

        void * readProcessChunk(pid_t pid, address_t addr, uint64_t bytes);

        template <typename T>
        void scanForData(T targetVal);
        template <typename T>
        void writeData(pid_t pid, address_t addr, T targetVal);

    public:
        MemoryScanner(pid_t pid, address_t, address_t);
        void changeDataType();
        void changeNumBytes();
        void changetargetValue();
        void scanMemory();
        void writeMemory();
        void clearContainer();
        void printContainer();
        bool isRunning();
        void killScanner();
        void printInfo();
};


#endif