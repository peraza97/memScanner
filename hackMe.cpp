#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char ** argv){

    printf("PID: %d\n",getpid());

    int health = 100;
    char letter = '$';
    char *str = new char[100];
    str[0]='l';str[1]='o';str[2]='v';str[3]='e';str[4]='\0';
    
    printf("%p: %d\n", &health, health);
    printf("%p: %c\n", &letter, letter); 
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
                printf("enter new value for str: ");
                scanf("%*c%[^\n]s", str);
                break;
            case 5:
                printf("health: %p\nletter: %p\nstr: %p\n", &health, &letter, str);
                break;
        }
    }
    return 0;
}
