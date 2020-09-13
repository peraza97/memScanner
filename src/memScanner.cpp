#include <sys/uio.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <errno.h>
#include <set>
#include <vector>
#include <iostream>

#include "scanner.h"

using namespace std;

void printMenu(){
    printf("-------------------------------\n");
    printf("0) Exit\n");
    printf("1) Print Known Addresses\n");
    printf("2) Clear Addresses\n");
    printf("3) Scan memory Region for int\n");
    printf("4) Scan memory Region for Char\n");
    printf("5) Scan memory Region for String\n");
    printf("6) Update Int Target Value\n");
    printf("7) Update Char Target Value\n");
    printf("8) Update String Target Value\n");
    printf("9) Write Value\n");
    printf("Enter Option: ");
}

void printAddressSet(container_t * mySet){
    int count = 0;
    if(mySet->size() == 0){
        printf("Container is empty\n");
        return;
    }
    printf("---------------\n");
    for(container_t::iterator it = mySet->begin(); it != mySet->end(); ++it){
        printf("%d) %p\n", count++, *it);
    }
    printf("---------------\n");
}

void clearSet(container_t * mySet){
    mySet->clear();
    printf("cleared Container\n");
}

address_t getSetAddress(container_t * mySet){
    printAddressSet(mySet);
    int index;
    printf("Enter address index: ");
    cin >> index;
    container_t::iterator addr = mySet->begin() + index;
    return *addr;
}

void updateAddress(pid_t pid, container_t * mySet, string targetString){
    if(mySet->size() == 0){
        return;
    }
    
    address_t addr = getSetAddress(mySet);

    printf("1)int\n2)char\n3)string\nEnter Datatype to write: ");
    int option;
    int newInt;
    char newChar;
    string newString;
    cin >> option;
    printf("Enter New value: ");
    switch (option){
    case 1:
        cin >> newInt;
        writeData(pid, addr, newInt);
        break;
    case 2:
        cin >> newChar;
        writeData(pid, addr, newChar);
        break;
    case 3:
        cin.ignore();
        getline(cin, newString);
        writeString(pid, addr, newString, targetString.size()+1);
        break;
    default:
        break;
    }
}

void optionHandler(int option, pid_t pid, address_t start, address_t end, container_t * mySet, int & targetInt, char & targetChar, string & targetString){
    switch (option){
    case 1:
        printAddressSet(mySet);
        break;
    case 2:
        clearSet(mySet);
        break;
    case 3:
        scanForData(pid, start, end, NUM_BYTES, targetInt, mySet);
        break;
    case 4:
        scanForData(pid, start, end, NUM_BYTES, targetChar, mySet);
        break;
    case 5:
        scanForString(pid, start, end, NUM_BYTES, targetString, mySet);
        break;
    case 6:
        printf("Enter new value for Int: ");
        cin >> targetInt;
        break;
    case 7:
        printf("Enter new value for Char: ");
        cin >> targetChar;
        break;
    case 8:
        printf("Enter new value for String: ");
        cin.ignore();
        getline(cin, targetString);
        break;
    case 9:
        updateAddress(pid, mySet, targetString);
        break;
    default:
        break;
    }
}

int main(){

    container_t * mySet = new container_t;
    pid_t pid = -1;
    address_t start = (address_t)0x0000000000000000;
    address_t end   = (address_t)0x00007fffffffffff;

    int targetInt = 100;
    char targetChar = '$';
    string targetString = "Hello world";

    printf("Enter process pid: ");
    cin >> pid;
    void * addr = readProcessChunk(pid,(address_t)0x7ffcabbb4280, sizeof(void*));
    uint64_t val = *(uint64_t*)(addr);
    printf("%p\n", val);
    addr = readProcessChunk(pid,(address_t)val, 8); //read the pointer
    writeString(pid, (address_t)val, "hello", 10);


    //writeString(pid, (address_t)addr, "hi", 100);
    
    /*
    int option = 6;
    while(option!= 0){
        printMenu();
        cin >> option;
        printf("\n");
        optionHandler(option, pid, start, end, mySet, targetInt, targetChar, targetString);
    }
    */
}