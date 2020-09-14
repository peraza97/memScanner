#include <iostream>
#include "memoryScanner.h"
using namespace std;

void printMenu(){
    printf("-------------------------------\n");
    printf("0) Exit\n");
    printf("1) Print Addresses\n");
    printf("2) Clear Addresses\n");
    printf("3) Scan memory\n");
    printf("4) Change Data type\n");
    printf("5) Update value to search for\n");
    printf("6) Write Value\n");
    printf("7) Change chunk size\n");
    printf("8) Print Info\n");
    printf("Enter Option: ");
}

int main(){

    pid_t pid = -1;
    printf("Enter process pid: ");
    cin >> pid;
    
    MemoryScanner myScanner(pid, (address_t)0x0000000000000000, (address_t)0x00007fffffffffff);
    myScanner.printInfo();

    int choice;
    while(myScanner.isRunning()){
        printMenu();
        cin >> choice;
        switch (choice){
            case 0:
                myScanner.killScanner();
                break;
            case 1:
                myScanner.printContainer();
                break;
            case 2:
                myScanner.clearContainer();
                break;
            case 3:
                myScanner.scanMemory();
                break;
            case 4:
                myScanner.changeDataType();
                break;
            case 5:
                myScanner.changetargetValue();
                break;
            case 6:
                myScanner.writeMemory();
                break;
            case 7:
                myScanner.changeNumBytes();
                break;
            case 8:
                myScanner.printInfo();
                break;
            default:
                myScanner.killScanner();
        }
    }

}
