#include "memoryScanner.h"

MemoryScanner::MemoryScanner(pid_t pid, address_t start, address_t end){
    this->running = true;
    this->container = new container_t;
    this->numBytesStr = "1MB";
    this->numBtyes = this->convertSizeInput(this->numBytesStr);
    this->dataType = INT;
    this->targetInt = 100;
    this->targetChar = '$';
    this->targetString = "Hack this stack string";

    //parameter init variables 
    this->startAddress = start;
    this->endAddress = end;
    this->pid = pid;
}

void MemoryScanner::printInfo(){
    printf("PID: %d\n", this->pid);
    printf("start address:%p\n", this->startAddress);
    printf("end address:%p\n", this->endAddress);
    printf("Chunk size: %s(%" PRIu64 ")\n", this->numBytesStr.c_str(), this->numBtyes);  
    switch (this->dataType){
        case INT:
            printf("target int: %d\n", this->targetInt);
            break;
        case CHAR:
            printf("target int: %c\n", this->targetChar);
            break;
        case STRING:
            printf("target string: %s\n", this->targetString.c_str());
            break;
    }   
    printf("----------------\n");

}

bool MemoryScanner::isRunning(){
    return this->running;
}

void MemoryScanner::killScanner(){
    this->running = false;
}

void MemoryScanner:: clearContainer(){
    this->container->clear();
}

void MemoryScanner:: printContainer(){
    int count = 0;
    if(this->container->size() == 0){
        printf("Container is empty\n");
        return;
    }
    printf("---------------\n");
    for(container_t::iterator it = this->container->begin(); it != this->container->end(); ++it){
        printf("%d) %p\n", count++, *it);
    }
    printf("---------------\n");
}

address_t MemoryScanner::getContainerIndex(){
    this->printContainer();
    int index;
    printf("Enter address index: ");
    cin >> index;
    container_t::iterator addr = this->container->begin() + index;
    return *addr;
}

datatype MemoryScanner::convertToDataType(int val){
    switch (val){
        case 1:
            return INT;
        case 2:
            return CHAR;
        case 3:
            return STRING;
        default:
            return INT;
    }
}

void MemoryScanner::changeDataType(){
    int option;
    printf("Enter what data type you want to look for\n1) Int\n2) Char\n3) String\nEnter option: ");
    cin >> option;
    if(option != this->dataType){
        this->clearContainer();
        this->dataType = this->convertToDataType(option);
    }
}

uint64_t MemoryScanner::convertSizeInput(string input){
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
    return (uint64_t)pow(2, bits);
}

void MemoryScanner::changeNumBytes(){
    printf("Enter new chunck size: ");
    cin >> this->numBytesStr;
    this->numBtyes = this->convertSizeInput(this->numBytesStr);

}

void MemoryScanner::changetargetValue(){
    switch (this->dataType){
    case INT:
        printf("Enter new value for Int: ");
        cin >> this->targetInt;
        break;
    case CHAR:
        printf("Enter new value for Char: ");
        cin >> this->targetChar;
        break;
    case STRING:
        printf("Enter new value for String: ");
        cin.ignore();
        getline(cin, this->targetString);
        break;
    default:
        break;
    }
}

void * MemoryScanner::readProcessChunk(pid_t pid, address_t addr, uint64_t bytes){
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
void MemoryScanner::scanForData(T targetVal){
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


void MemoryScanner::scanMemory(){
    switch (this->dataType){
        case INT:
            cout << "Scanning for value: " << this->targetInt << endl;
            this->scanForData(this->targetInt);
            break;
        case CHAR:
            cout << "Scanning for value: " << this->targetChar << endl;
            this->scanForData(this->targetChar);
            break;
        case STRING:
            cout << "Scanning for value: " << this->targetString << endl;
            this->scanForData(this->targetString);
            break;
        default:
            break;
    }
    cout << "Known addresss with this value: " << this->container->size() << endl;
}

template <typename T>
void MemoryScanner::writeData(pid_t pid, address_t addr, T targetVal){

    cout << sizeof(targetVal) << endl;
    struct iovec local_iov[1];

    local_iov[0].iov_base = &targetVal;
    local_iov[0].iov_len = sizeof(targetVal);

    struct iovec remote_iov[1];
    remote_iov[0].iov_base = addr;
    remote_iov[0].iov_len = sizeof(targetVal);

    ssize_t nread = process_vm_writev(pid, local_iov, 1, remote_iov, 1, 0);
    success(nread);
}

template <>
void MemoryScanner::writeData<string>(pid_t pid, address_t addr, string targetVal){
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

void MemoryScanner::writeMemory(){

    if(this->container->size() == 0){
        return;
    }
    address_t addr = getContainerIndex();
    switch (this->dataType){
        case INT:
            writeData(this->pid, addr, targetInt);
            break;
        case CHAR:
            writeData(this->pid, addr, targetChar);
            break;
        case STRING:
            writeData(this->pid, addr, targetString);
            break;
        default:
            break;
    }
}