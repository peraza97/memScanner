#ifndef __UTIL_H__
#define __UTIL_H__

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

typedef uint8_t * address_t;
typedef vector<address_t> container_t;

enum datatype{INT, CHAR, STRING};

template <typename T>
int iterateData(T data){
    return sizeof(data);
}

template<>
int iterateData<string>(string data);

template <typename T>
int sizeofData(T data){
    return sizeof(data);
}

template<>
int sizeofData<string>(string data);

template <typename T>
bool compareData(address_t ptr, T targetVal){
    T tempVal = *(T*)ptr;
    return tempVal == targetVal;
}

//specific to string
template<>
bool compareData<string>(address_t ptr, string targetVal);

template <typename T>
void editData(T & targetVal){
    printf("New value: ");
    cin >> targetVal;
}

//specific to string
template<>
void editData<string>(string & targetVal);

bool success(ssize_t nread);

#endif 