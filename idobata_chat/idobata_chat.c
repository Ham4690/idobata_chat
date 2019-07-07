/*
  idobata_chat.c
*/

#include "mynet.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define SERVER_LEN 256     /* サーバ名格納用バッファサイズ */
#define DEFAULT_PORT 50001 /* ポート番号既定値 */
#define NAMELENGTH 20 //username 長さ制限
#define S_BUFSIZE 512
#define R_BUFSIZE 512
#define TIMEOUT_SEC 10 

#define DEFAULT_MODE 'C'   /* 省略時はクライアント */

void argc_error_check(int arg_num,char *argv[],int *port);
void user_name_check(char *input_name,char *name);

int main(int argc, char *argv[])
{

  int port_number=DEFAULT_PORT;
  char servername[SERVER_LEN] = "localhost";
  char mode = DEFAULT_MODE;
  char name[NAMELENGTH];

  struct sockaddr_in broadcast_adrs ;
  struct sockaddr_in from_adrs;
  socklen_t from_len;

  int sock;
  int broadcast_sw=1;
  fd_set mask, readfds;
  struct timeval timeout;

  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;
  int count = 1;

  argc_error_check(argc,argv,&port_number);
  user_name_check(argv[1],name);
/////
  if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, 
            (void *)&broadcast_sw, sizeof(broadcast_sw)) == -1){
    exit_errmesg("setsockopt()");
  }

  set_sockaddr_in_broadcast(&broadcast_adrs,(in_port_t)port_number); 
  sock = init_udpclient();
                                          
  /* ビットマスクの準備 */
  FD_ZERO(&mask);               
  FD_SET(sock, &mask);

  /* キーボードから文字列を入力する */
  s_buf[0]="m" ;
  strsize = strlen(s_buf);


  /* 文字列をサーバに送信する */
  Sendto(sock, s_buf, strsize, 0, 
	(struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs) );

  readfds = mask;
  timeout.tv_sec = TIMEOUT_SEC;
  timeout.tv_usec = 0;
 
  /* サーバから文字列を受信して表示 */
  for(;;){
    /* 受信データの有無をチェック */
     readfds = mask;
     timeout.tv_sec = TIMEOUT_SEC;
     timeout.tv_usec = 0; 

    if( select( sock+1, &readfds, NULL, NULL,&timeout)==0 ){
        // printf("Time out.\n");
        // break;
       if(count >= 3){
         printf("Time out.\n");
         break;
       }
       printf("count_time = %d\n",count++);
       Sendto(sock, s_buf, strsize, 0, 
       (struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs) );     
       memcpy(&readfds,&mask,sizeof(fd_set));
    }

    // from_len = sizeof(from_adrs);
    // strsize = Recvfrom(sock, r_buf, R_BUFSIZE-1, 0,
		//      (struct sockaddr *)&from_adrs, &from_len);
    // r_buf[strsize] = '\0';
    // printf("[%s] %s",inet_ntoa(from_adrs.sin_addr), r_buf);
  }

  close(sock);             /* ソケットを閉じる */
/////

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
