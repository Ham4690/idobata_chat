#include <stdio.h>
#include <string.h>
#define MESSAGE 3

int main(){
    char s_buf[10];
    fgets(s_buf,10, stdin);
    printf("%d\n",strlen(s_buf));
    printf("%s",s_buf);
}