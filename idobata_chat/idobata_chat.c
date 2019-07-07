/*
  idobata_chat.c
*/

#include "mynet.h"
#include <stdlib.h>
#include <unistd.h>

#define SERVER_LEN 256     /* サーバ名格納用バッファサイズ */
#define DEFAULT_PORT 50001 /* ポート番号既定値 */
#define NAMELENGTH 20 //username 長さ制限
// #define DEFAULT_NCLIENT 3  /* 省略時のクライアント数 */
// #define DEFAULT_MODE 'C'   /* 省略時はクライアント */

// extern char *optarg;
// extern int optind, opterr, optopt;

void argc_error_check(int arg_num,char *argv[],int *port);
void user_name_check(char *input_name,char *name);

int main(int argc, char *argv[])
{

  int port_number=DEFAULT_PORT;
//   int num_client =DEFAULT_NCLIENT;
  char servername[SERVER_LEN] = "localhost";
//   char mode = DEFAULT_MODE;
  char name[NAMELENGTH];
  char mode;
	
  argc_error_check(argc,argv,&port_number);

  if(argc == 3){
  	port_number=atoi(argv[2]);
  }
  
  user_name_check(argv[1],name);
  printf("port_num = %d\n",port_number);
  printf("user_name = %s\n",name);
  

//   switch(mode){

//   case 'S':
//     // chat_server(port_number, num_client);  /* サーバ部分ができたらコメントを外す */
//     chat_server(port_number);  /* サーバ部分ができたらコメントを外す */
//     break;
//   case 'C':
//     chat_client(servername, port_number);
//     break;

//   }

   exit(EXIT_SUCCESS);
}


void argc_error_check(int arg_num ,char *argv[],int *port){
  if(arg_num > 3){
	  printf("Number of argument is too many.\n");
	  exit(EXIT_FAILURE);
  } else if(arg_num == 1){
	  printf("Set the username as an argument.\n");
	  exit(EXIT_FAILURE);
  }

  if(arg_num== 3){
  	*port=atoi(argv[2]);
  }
}

void user_name_check(char *input_name,char *name){
  if(strlen(input_name) > NAMELENGTH){
	  printf("error name_length\n");
	  exit(EXIT_FAILURE);
  }
  sprintf(name,"%s",input_name);
}
