#include <iostream>
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);
using namespace std;


int main(){

    cout << "PID: " << getpid() << endl;

    int health = 100;
    char letter = '$';
    char str[] = "Hello world";
    
    cout << (void *)&health << ": " << health << endl;
    cout << (void *)&letter << ": " << letter << endl;    
    printf("%p: %s\n", &str, str); 

    int pressed = 1;
    while(pressed != 0){
        printf("\n0) Quit\n1) Print all variables\n2) Edit health\n3) Edit letter\n4) Edit string\n5) Print addresses\nEnter Option: ");
        scanf("%d", &pressed);
        printf("--------------------\n");
        switch (pressed){
            case 1:
                printf("health: %d\nletter: %c\nstr: %s\n", health, letter, str);
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
                break;
            case 5:
                printf("health: %p\nletter: %p\nstr: %p\n", &health, &letter, str);
                break;
        }
    }

}
