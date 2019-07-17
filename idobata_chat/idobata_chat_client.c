/*
  chat_client.c
*/

#include "idobata.h"
#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>

#define S_BUFSIZE 512 /* 送信用バッファサイズ */
#define R_BUFSIZE 512 /* 受信用バッファサイズ */
// *name = 各ユーザの名前

void JOIN_msg_send(int sock,char *name);
void QUIT_msg_send(int sock);
void POST_msg_send(int sock,char *s_buf);
void MESG_msg_display(char *r_buf);
void POST_msg_display(char *r_buf);
void LOGIN_msg_send(int sock);
void Tag_remove_display(char *r_buf);
void Input_msg(char *s_buf);
void packet_check(char *r_buf,int sock);
void server_error_check(int strsize);
void output_tab(int num);


void idobata_chat_client(char* servername,int port_number,char *name)
{
  int sock;
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;
  int check_msg;
  fd_set mask, readfds;
  struct timeval tv;

  /* サーバに接続する */
  sock = init_tcpclient(servername,port_number);

  printf("Hello. Welcome idobata_chat [%s].\n",name);

  //{JOIN username を送信}
  JOIN_msg_send(sock,name);

  // /* ビットマスクの準備 */

  // bit 0 :標準入出力を確認

  FD_ZERO(&mask);
  FD_SET(0, &mask);
  FD_SET(sock,&mask);
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  readfds = mask;
  select(sock+1,&readfds,NULL,NULL,&tv);

  LOGIN_msg_send(sock);

// サーバと接続しているソケットを監視
  for(;;){

    memset(r_buf,'\0',R_BUFSIZE);
    memset(s_buf,'\0',S_BUFSIZE);
    /* 受信データの有無をチェック */

    readfds = mask;
    select(sock+1,&readfds,NULL,NULL,&tv);

//送信
//自身のキーボード入力チェック
    if( FD_ISSET(0, &readfds)){
      fgets(s_buf, S_BUFSIZE, stdin);
      s_buf[strlen(s_buf)] = '\0';

      //packet識別,送信
      packet_check(s_buf,sock);
//先頭タグの確認
    }

//受信
    if( FD_ISSET(sock,&readfds ) ){
      //サーバが終了してたらエラー表示後、プログラムを終了する。
      //packet受信
      strsize = Recv(sock,r_buf,R_BUFSIZE-1,0);
      if(strsize == 0){
        printf("\nserver error\n");
        exit(1);
      }
      //サーバが終了していないか確認
      fflush(stdout);
      //パケット識別、出力
      packet_check(r_buf,sock);
    }
  }
}

void Tag_remove_display(char *r_buf){
  int i;
  int strsize;

  strsize = strlen(r_buf);
  r_buf[strsize] = '\n';
  for(i=5;i<strsize;i++){
    printf("%c",r_buf[i]);
  }
  printf("\n");
  fflush(stdout); /* バッファの内容を強制的に出力 */
}

void output_tab(int num){
  int i;
  if(num < 0){
    printf("error:output_tab()\n");
  }else{
    for(i=0;i<num;i++){
      printf("        ");
    }
  }
}

void JOIN_msg_send(int sock,char *name){
  char JOIN_packet[20];
  int strsize;
  strcpy(JOIN_packet,name);
  create_packet(JOIN,JOIN_packet);
  strsize = strlen(JOIN_packet);
  Send(sock,JOIN_packet,strsize,0);
}

void QUIT_msg_send(int sock){
  char QUIT_packet[5];
  int strsize;
  create_packet(QUIT,QUIT_packet);
  strsize = strlen(QUIT_packet);
  Send(sock, QUIT_packet, strsize, 0);
  printf("ok\n");
  exit(1);
}

void POST_msg_send(int sock,char *s_buf){
  int strsize;
  create_packet(POST,s_buf);
  strsize = strlen(s_buf);
  Send(sock, s_buf,strsize,0); 
}

void server_error_check(int strsize){
  if(strsize == 0){
    printf("\nserver error\n");
    exit(1);
  }
}

void MESG_msg_display(char *r_buf){
  output_tab(5);
  Tag_remove_display(r_buf);
}

void LOGIN_msg_send(int sock){
  char msg[15];
  strcpy(msg,"--login--");
  POST_msg_send(sock,msg);
}

void POST_msg_display(char *r_buf){
  output_tab(2);
  Tag_remove_display(r_buf);
}

void Input_msg(char *s_buf){
  int Input_msg_check = 1;  
  fgets(s_buf, S_BUFSIZE, stdin);
}


void packet_check(char *msg,int sock){
  int packet_mode;
  //届いたmsgのタグ確認
  packet_mode = analyze_header(msg);
  switch (packet_mode)
  {
  case MESSAGE: 
    MESG_msg_display(msg);
    break;

  case POST: 
    POST_msg_display(msg);
    break;

  case QUIT:
    QUIT_msg_send(sock);

  default:
    POST_msg_send(sock,msg);
    break;
  }
}

