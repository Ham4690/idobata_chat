#include <stdio.h>
#include <unistd.h>
#include <string.h>

void swp(int *x,int *y){
    int swp;
    swp = *x;
    *x = *y;
    *y = swp;
}

int main(int argc, char* argv[])
{
    char c[100];
    char *p;
    int x = 2;
    int y = 3;
    int *px,*py;
    printf("x,y=%d,%d\n",x,y);
    swp(&x,&y);
    printf("x,y=%d,%d\n",x,y);
    // printf("%s",argv[1]);
    // printf(" %d\n",strlen(argv[1]));
    // if(strlen(argv[1]) < 4){
    //     printf("error\n");
    // }else{
    //     printf("ok\n");
    // }
    // sprintf(c,"%s",argv[1]);
    // p = c;
    // printf("%s\n",p);
    // printf("%d\n",strlen(p));
    return 0;
}

