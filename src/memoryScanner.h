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

template <class myType>
class MemoryScanner{
    private:
        //scanner variables
        pid_t pid;
        container_t * container;
        string numBytesStr;
        uint64_t numBtyes;
        address_t startAddress;
        address_t endAddress;
        bool running;

        //target variables 
        myType targetData;

        //helper functions

        uint64_t convertSizeInput(string);

        address_t getContainerIndex();

        void * readProcessChunk(pid_t pid, address_t addr, uint64_t bytes);

        void scanForData(myType targetVal);
        void writeData(pid_t pid, address_t addr, myType targetVal);

    public:
        MemoryScanner(pid_t pid, address_t, address_t);
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

template <class myType>
MemoryScanner<myType>::MemoryScanner(pid_t pid, address_t start, address_t end){
    this->running = true;
    this->container = new container_t;
    editData(this->targetData);
    this->numBytesStr = "1MB";
    this->numBtyes = this->convertSizeInput(this->numBytesStr);

    //parameter init variables 
    this->startAddress = start;
    this->endAddress = end;
    this->pid = pid;
}

template <class myType>
void MemoryScanner<myType>::printInfo(){
    printf("PID: %d\n", this->pid);
    printf("start address:%p\n", this->startAddress);
    printf("end address:%p\n", this->endAddress);
    printf("Chunk size: ~%s(%" PRIu64 ")\n", this->numBytesStr.c_str(), this->numBtyes);  
    cout << "Data: (" << this->targetData <<")" << endl;
    cout << "size of data: " << sizeofData(this->targetData) << endl;
    cout << "iterate size: " << iterateData(this->targetData) << endl;
}

template <class myType>
bool MemoryScanner<myType>::isRunning(){
    return this->running;
}

template <class myType>
void MemoryScanner<myType>::killScanner(){
    this->running = false;
}

template <class myType>
void MemoryScanner<myType>:: clearContainer(){
    this->container->clear();
}

template <class myType>
void MemoryScanner<myType>::printContainer(){
    int count = 0;
    if(this->container->size() == 0){
        printf("Container is empty\n");
        return;
    }
    printf("-------------------------------\n");
    for(container_t::iterator it = this->container->begin(); it != this->container->end(); ++it){
        printf("%d) %p\n", count++, *it);
    }
    printf("-------------------------------\n");
}

template <class myType>
address_t MemoryScanner<myType>::getContainerIndex(){
    this->printContainer();
    int index;
    printf("Enter address index: ");
    cin >> index;
    container_t::iterator addr = this->container->begin() + index;
    address_t returnAddr = *addr;
    this->container->erase(addr);
    return returnAddr;
}

template <class myType>
uint64_t MemoryScanner<myType>::convertSizeInput(string input){
    uint32_t result = 0;
    int position = 0;
    for(position = 0; position < input.size(); ++position){
        if(!isdigit(input.at(position))){
            break;
        }
    }
    result = atoi(input.substr(0,position).c_str());
    if(ceil(log2(result)) != floor(log2(result))){
        return 0;
    }
    int bits = log2(result);
    string suffix = input.substr(position);
    if(suffix == "" || suffix == "B"){
        bits += 0;
    }
    else if(suffix == "KB"){
        bits += 10;
    }
    else if(suffix == "MB"){
        bits += 20;
    }
    else if(suffix == "GB"){
        bits += 30;
    }
    else{
        return 0;
    }
    uint64_t size = (uint64_t)pow(2, bits);
    return int(size/sizeofData(this->targetData)) * sizeofData(this->targetData); //round to get correct sizing
}

template <class myType>
void MemoryScanner<myType>::changeNumBytes(){
    printf("Enter new chunck size: ");
    cin >> this->numBytesStr;
    this->numBtyes = this->convertSizeInput(this->numBytesStr);

}

template<class myType>
void MemoryScanner<myType>::changetargetValue(){
    editData(this->targetData);
}

template<class myType>
void * MemoryScanner<myType>::readProcessChunk(pid_t pid, address_t addr, uint64_t bytes){
    struct iovec local_iov[1];
    local_iov[0].iov_base = new uint8_t[bytes];
    local_iov[0].iov_len = bytes;
    struct iovec remote_iov[1];
    remote_iov[0].iov_base = addr;
    remote_iov[0].iov_len = bytes;
    ssize_t nread = process_vm_readv(pid, local_iov, 1, remote_iov, 1, 0);
    if(!success(nread)){ //if failed, deallocate the memory
        delete[] (address_t)local_iov[0].iov_base;
        return NULL;
    }
    return local_iov[0].iov_base;
}

template <class myType>
void MemoryScanner<myType>::scanForData(myType targetVal){
    if(this->container->size() == 0){ 
        address_t start;
        for(start = this->startAddress; start <= this->endAddress; start += this->numBtyes){
            address_t ptr = (address_t)readProcessChunk(pid, start, this->numBtyes);
            if(ptr != NULL){
                for(address_t tmp = ptr; tmp < ptr + this->numBtyes; tmp += iterateData(targetVal)){
                    if(compareData(tmp, targetVal)){
                        long diff = tmp - ptr;
                        this->container->push_back(start + diff);
                    }
                }
                delete[] ptr;
            }
        }
    }
    else{ //only check known addresses
        for(container_t::iterator it = this->container->begin(); it != this->container->end();){
            address_t ptr = (address_t)readProcessChunk(pid, *it, sizeofData(targetVal));
            if((ptr!=NULL) && !(compareData(ptr, targetVal))){
                this->container->erase(it);
            }
            else{
                ++it;
            }
        }
    }
}

template<class myType>
void MemoryScanner<myType>::scanMemory(){
    cout << "Scanning for value: " << this->targetData << endl;
    this->scanForData(this->targetData);
    cout << "Known addresss with this value: " << this->container->size() << endl;
}

template <class myType>
void MemoryScanner<myType>::writeData(pid_t pid, address_t addr, myType targetVal){
    struct iovec local_iov[1];

    local_iov[0].iov_base = &targetVal;
    local_iov[0].iov_len = sizeof(targetVal);

    struct iovec remote_iov[1];
    remote_iov[0].iov_base = addr;
    remote_iov[0].iov_len = sizeof(targetVal);

    ssize_t nread = process_vm_writev(pid, local_iov, 1, remote_iov, 1, 0);
    success(nread);
}

template<>
void MemoryScanner<string>::writeData(pid_t pid, address_t addr, string targetVal){
    struct iovec local_iov[1];
    char * newStr = new char[targetVal.size()+1];
    strcpy(newStr, targetVal.c_str());

    local_iov[0].iov_base = newStr;
    local_iov[0].iov_len = targetVal.size()+1;

    struct iovec remote_iov[1];
    remote_iov[0].iov_base = addr;
    remote_iov[0].iov_len = targetVal.size()+1;
    ssize_t nread = process_vm_writev(pid, local_iov, 1, remote_iov, 1, 0);
    delete[] newStr;
}

template<class myType>
void MemoryScanner<myType>::writeMemory(){
    if(this->container->size() == 0){
        return;
    }
    address_t addr = getContainerIndex();
    myType newValue;
    editData(newValue);
    this->writeData(this->pid, addr, newValue);
}


#endif