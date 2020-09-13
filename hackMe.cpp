#include <stdio.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char ** argv){

    printf("PID: %d\n",getpid());

    int health = 100;
    char letter = '$';
    char stackStr[] = "Dummy";
    char * heapStr = new char[100];
    strcpy(heapStr, "love");
    
    printf("%p: %d\n", &health, health);
    printf("%p: %c\n", &letter, letter); 
    printf("%p: %s\n", &stackStr, stackStr); 
    printf("%p: %s\n", heapStr, heapStr); 

    int pressed = 1;
    while(pressed != 0){
        printf("\n0) Quit\n1) Print all variables\n2) Edit health\n3) Edit letter\n4) Edit string\n5) Print addresses\nEnter Option: ");
        scanf("%d", &pressed);
        printf("--------------------\n");
        switch (pressed){
            case 1:
                printf("health: %d\nletter: %c\nstackStr: %s\nheapStr: %s\n", health, letter, stackStr, heapStr);
                break;
            case 2:
                printf("enter new value for health: ");
                scanf("%d", &health);
                break;
            case 3:
                printf("enter new value for letter: ");
                scanf(" %c", &letter);
                break;
            case 4:
                printf("enter new value for heapStr: ");
                scanf("%*c%[^\n]s", heapStr);
                break;
            case 5:
                printf("health: %p\nletter: %p\nstackStr: %p\nheapStr: %p\n", &health, &letter, &stackStr, heapStr);
                break;
        }
    }
    return 0;
}
