#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
// #include "mynet.h"

#define MSGBUF_SIZE 512

static char Buffer[MSGBUF_SIZE];


void swp(int *x,int *y){
    int swp;
    swp = *x;
    *x = *y;
    *y = swp;
}

void create_packet(int type, char *message )
{
    char *keep_msg;
    keep_msg = (char *)malloc((strlen(message)+5) * sizeof(char));

    switch( type ){
    case 1:
    strcpy(keep_msg,"HELO ");
    strncat(keep_msg,message,strlen(message));
    snprintf( message, MSGBUF_SIZE , "%s",keep_msg );
    free(keep_msg);
    break;
    case 2:
    snprintf( Buffer, MSGBUF_SIZE, "JOIN %s", message );
    break;
    default:
    /* Undefined packet type */
    break;
    }

}

int main(int argc, char* argv[])
{
    char c[100];

    strcpy(c,"test");
    // printf("%lu\n",strlen(c));
    create_packet(1,c);
    printf("%s\n",c);

    return 0;
}

