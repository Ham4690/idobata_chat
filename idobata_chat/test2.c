#include <stdio.h>
#include <string.h>



int main(){
    char buf[20];
    char buf_2[30];

    strcpy(buf_2,"HELO ");
    snprintf(buf,10,"%s","123456789012345");
    strncat(buf_2,buf,strlen(buf));
    printf("%s\n",buf);
    printf("%s\n",buf_2);

    return 0;
}