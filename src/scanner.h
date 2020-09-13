#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
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

template <typename T>
int iterateData(T data){
    return sizeof(data);
}

template<>
int iterateData<string>(string data){
    return 1;
}

template <typename T>
int sizeofData(T data){
    return sizeof(data);
}

template<>
int sizeofData<string>(string data){
    return data.size()+1;
}

template <typename T>
bool compareData(address_t ptr, T targetVal){
    T tempVal = *(T*)ptr;
    return tempVal == targetVal;
}

//specific to string
template<>
bool compareData<string>(address_t ptr, string targetVal){
    return strcmp((char*)ptr, targetVal.c_str()) == 0;
}

template <typename T>
void scanForData(pid_t pid, address_t start, address_t end, uint64_t bytes, T targetVal, container_t * mySet){
    cout << "Scanning for target: " << targetVal << endl;
    if(mySet->size() == 0){ 
        for(start; start <= end; start += bytes){
            address_t ptr = (address_t)readProcessChunk(pid, start, bytes);
            if(ptr != NULL){
                for(address_t tmp = ptr; tmp <= ptr + bytes; tmp += iterateData(targetVal)){
                    if(compareData(tmp, targetVal)){
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
            address_t ptr = (address_t)readProcessChunk(pid, *it, sizeofData(targetVal));
            if((ptr!=NULL) && (!compareData(ptr, targetVal))){
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

template <>
void writeData<string>(pid_t pid, address_t addr, string targetVal){
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

#endif