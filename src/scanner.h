#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <set>
#include <vector>
#include <iostream>

using namespace std;

typedef uint8_t * address_t;
typedef vector<address_t> container_t;

#define KB 1024
#define MB 1048576
#define GB 1073741824 

#define NUM_BYTES 4 * MB

bool success(ssize_t nread){
    if (nread < 0) {
        switch (errno) {
            case EINVAL:
              printf("ERROR: INVALID ARGUMENTS.\n");
              break;
            case EFAULT:
              printf("ERROR: UNABLE TO ACCESS TARGET MEMORY ADDRESS.\n");
              break;
            case ENOMEM:
              printf("ERROR: UNABLE TO ALLOCATE MEMORY.\n");
              break;
            case EPERM:
              printf("ERROR: INSUFFICIENT PRIVILEGES TO TARGET PROCESS.\n");
              break;
            case ESRCH:
              printf("ERROR: PROCESS DOES NOT EXIST.\n");
              break;
            default:
              printf("ERROR: AN UNKNOWN ERROR HAS OCCURRED.\n");
              break;
        }
        return false;
    }
    return true;
}

//READ X bytes from remote process
void * readProcessChunk(pid_t pid, address_t addr, uint64_t bytes){
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

//Template for scanning data types
template <typename T>
void scanForData(pid_t pid, address_t start, address_t end, uint64_t bytes, T targetVal, container_t * mySet){
    cout << "Scanning for target: " << targetVal << endl;
    if(mySet->size() == 0){ //no starting point, so scan all address space
        for(start; start <= end; start += bytes){
            address_t ptr = (address_t)readProcessChunk(pid, start, bytes);
            if(ptr != NULL){ //verify that we were able to get this memory
                for(address_t tmp = ptr; tmp <= ptr + bytes; tmp += sizeof(T)){
                    T tempVal = *(T*)tmp;
                    if(tempVal == targetVal){
                        long diff = tmp - ptr;
                        mySet->push_back(start + diff);
                    }
                }
                delete[] ptr;
            }
        }
    }
    else{ //only check known addresses
        for(container_t::iterator it = mySet->begin(); it != mySet->end();){
            T* ptr = (T*)readProcessChunk(pid, *it, 4);
            if((ptr!=NULL) && (*ptr != targetVal)){
                mySet->erase(it);
            }
            else{
                ++it;
            }
        }
    }
    cout << "There are " << mySet->size() << " addresses with this value" << endl;
}

void scanForString(pid_t pid, address_t start, address_t end, uint64_t bytes, string targetVal, container_t * mySet){
    cout << "Scanning for target: " << targetVal.c_str() << endl;
    if(mySet->size() == 0){ //no starting point, so scan all address space
        for(start; start <= end; start += bytes){
            address_t ptr = (address_t)readProcessChunk(pid, start, bytes);
            if(ptr != NULL){ //verify that we were able to get this memory
                for(char * tempString = (char*)ptr; (address_t)tempString <= ptr + bytes; tempString += 1){
                    if(strcmp(tempString, targetVal.c_str()) == 0){
                        cout << "HERE" << endl;
                        long diff = (address_t)tempString - ptr;
                        mySet->push_back(start + diff);
                    }
                }
                delete[] ptr;
            }
        }
    }
    else{ //only check known addresses
        for(container_t::iterator it = mySet->begin(); it != mySet->end();){
            char * ptr = (char*)readProcessChunk(pid, *it, targetVal.size()+1);
            if((ptr!=NULL) && (strcmp(ptr, targetVal.c_str()) != 0)){
                mySet->erase(it);
            }
            else{
                ++it;
            }
        }
    }
    cout << "There are " << mySet->size() << " addresses with this value" << endl;
}

template <typename T>
void writeData(pid_t pid, address_t addr, T targetVal){
    struct iovec local_iov[1];

    local_iov[0].iov_base = &targetVal;
    local_iov[0].iov_len = sizeof(targetVal);

    struct iovec remote_iov[1];
    remote_iov[0].iov_base = addr;
    remote_iov[0].iov_len = sizeof(targetVal);

    ssize_t nread = process_vm_writev(pid, local_iov, 1, remote_iov, 1, 0);
}

void writeString(pid_t pid, address_t addr, string targetVal, int prevSize){
    struct iovec local_iov[1];

    char * newStr = new char[prevSize];
    strcpy(newStr, targetVal.c_str());

    local_iov[0].iov_base = newStr;
    local_iov[0].iov_len = prevSize;

    struct iovec remote_iov[1];
    remote_iov[0].iov_base = addr;
    remote_iov[0].iov_len = prevSize;
    ssize_t nread = process_vm_writev(pid, local_iov, 1, remote_iov, 1, 0);
    delete[] newStr;
}

#endif