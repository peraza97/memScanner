#include "util.h"

template<>
int iterateData<string>(string data){
    return 1;
}

template<>
int sizeofData<string>(string data){
    return data.size()+1;
}

template<>
bool compareData<string>(address_t ptr, string targetVal){
    return strcmp((char*)ptr, targetVal.c_str()) == 0;
}

template<>
void editData<string>(string & targetVal){
    printf("New value: ");
    cin.ignore();
    getline(cin, targetVal); 
}


bool success(ssize_t nread){
    if (nread < 0) {
        switch (errno) {
            case EINVAL:
              printf("ERROR: INVALID ARGUMENTS.\n");
              break;
            case EFAULT:
              //printf("ERROR: UNABLE TO ACCESS TARGET MEMORY ADDRESS.\n");
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