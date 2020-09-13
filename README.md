## Mem Scanner

MemScanner is a simple playground tool thats allows a user to read and modify the memory of another running process.
Given a target Valiue, it scans the address space for matching values, storing these addresses. In the case of multiple passes, 
multiple passes are needed to narrow down the results. Once an address is found, the contents of the address can be modified to your liking.

Currently this works on data types
- string
- int
- char

## Installation

To compile the project, run the following command in the top level directory
```sh
make
```
You will also need to compile the hackMe program
```sh
g++ hackMe.cpp
```

## Usage
run the following command in the top level directory
In a separate terminal run the following command in the top level directory
```sh
$./a.out
```

In a separate terminal run the following command in the top level directory
```sh
$make run
```

## To Do
- Update memory scan for strings in the case only part of the string is grabbed
- Update memory scan to handle more data types
- Make chunk size modular. Depending on chunk size, value may not be found