#include<stdio.h>
#include<stdlib.h>

int main(){
    char msg[20];
    char buf[20] = " test\n";
    // strcpy(msg,"hello%d",1);
    sprintf(msg,"hello%d",1);
    strcat(msg,buf);
    printf("%s",msg);
}