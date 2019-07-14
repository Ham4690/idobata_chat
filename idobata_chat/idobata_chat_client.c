/*
  chat_client.c
*/

#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>

#define S_BUFSIZE 100   /* 送信用バッファサイズ */
#define R_BUFSIZE 100   /* 受信用バッファサイズ */
// *name = 書くユーザの名前

void QUIT_msg_send(int sock);
void POST_msg_send(int sock,char *s_buf);
void MESG_msg_display(char *r_buf);
void server_error_check(int sock,char *r_buf);

void idobata_chat_client(char* servername,int port_number,char *name)
{
  int sock;
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;
  int check_msg;
  fd_set mask, readfds;

  /* サーバに接続する */
  sock = init_tcpclient(servername,port_number);

  printf("Connected.\n");
  //{JOIN username を送信}
//   /* ビットマスクの準備 */
  // bit 0 :標準入出力を確認

  FD_ZERO(&mask);
  FD_SET(0, &mask);
  FD_SET(sock,&mask);

// サーバと接続しているソケットを監視
  for(;;){

    /* 受信データの有無をチェック */
    readfds = mask;
    select(sock+1,&readfds,NULL,NULL,NULL);

// 自身のキーボードからの入力を監視
    if( FD_ISSET(0, &readfds) ){
      /* キーボードから文字列を入力する */
      fgets(s_buf, S_BUFSIZE, stdin);

//先頭タグの確認
      check_msg = analyze_header(s_buf);
      
      if( check_msg == QUIT){
        //QUITメッセージをサーバに送る
        QUIT_msg_send(sock);

        // strsize = strlen(s_buf);
        // Send(sock, s_buf, strsize, 0);
        // printf("ok\n");
        // exit(1);

      }else{
//入力されたテキストが「QUIT」でなければそれを「POST 発言メッセージ」の形式でサーバに送信する。
        POST_msg_send(sock,s_buf);
        // create_packet(POST,s_buf);
        // strsize = strlen(s_buf);
        // Send(sock, s_buf,strsize,0); 
      }
    }

//「MESG 発言メッセージ」形式の メッセージを受信したら、その「発言メッセージ」を画面に表示する。
    if( FD_ISSET(sock,&readfds ) ){
      /* サーバから文字列を受信する */
      // strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);
      //サーバが終了してたらエラー表示後、プログラムを終了する。
      server_error_check(strsize);
      check_msg = analyze_header(r_buf);

      if(check_msg == MESG){
        MESG_msg_display(r_buf);
        // r_buf[strsize] = '\0';
        // printf("%s",r_buf);
        // fflush(stdout); /* バッファの内容を強制的に出力 */

      }
    }
  }

}


void QUIT_msg_send(int sock){
  int strsize;
  char q_msg;
  create_packet(QUIT,q_msg);
  strsize = strlen(q_msg);
  Send(sock, q_msg, strsize, 0);
  printf("ok\n");
  exit(1);
}

void POST_msg_send(int sock,char *s_buf){
  int strsize;
  create_packet(POST,s_buf);
  strsize = strlen(s_buf);
  Send(sock, s_buf,strsize,0); 
}

void server_error_check(int sock,char *r_buf){
  int strsize;
  strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);
  if(strsize == 0){
    printf("\nserver error\n");
    exit(1);
  }
}

void MESG_msg_display(char *r_buf){
  int strsize;
  strsize = strlen(r_buf);
  r_buf[strsize] = '\0';
  printf("%s",r_buf);
  fflush(stdout); /* バッファの内容を強制的に出力 */
}