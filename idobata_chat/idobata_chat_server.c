/*
  chat_util.c
*/

#include "idobata.h"
#include "mynet.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#define NAMELENGTH 20 /* ログイン名の長さ制限 */
#define BUFSIZE 500    /* 通信バッファサイズ */
#define L_USERNAME 15  /* ユーザー名の長さ制限 */

/* ユーザーの情報を管理する線形リスト */
typedef struct user_info {
  char username[L_USERNAME];     /* ユーザ名 */
  int  sock;                     /* ソケット番号 */
  struct user_info *next;        /* 次のユーザ */
} User_info;

static User_info server = {"server", 0, NULL};  //アカウント管理用線形リストの先頭を指す.
static User_info *start = &server;
struct idobata_packet *packet;  //パケットの管理用
int Max_sd;
char r_buf[BUFSIZE];  //受信パケットを格納.
char s_buf[BUFSIZE];  //送信パケットを格納.

/* プライベート関数 */
static int max(int a, int b); //2つの数の内大きいほうの値を返す.
static void manage_login(int sock_listen);
//static int client_login(int sock_listen);  //ログイン時の処理を行う.
static char *chop_nl(char *s);  //改行文字を消す.
void create_message(int sock_detect);  //[username] message 形式のメッセージをs_bufに格納する.
static void send_to_others(int my_sock); //my_sock以外のソケット番号を持つクライアントにs_bufを送信.
static void append_node(int sock_accept);  //先頭にノードを追加する(JOIN用)
void HELLO_process(int udp_sock, const struct sockaddr *to, socklen_t tolen);
void JOIN_process(int sock_detect);  //構造体にusername情報を追加する.
void POST_process(int sock_detect);
void QUIT_process(int sock_detect);  //QUITを送られた後のプロセス

void idobata_chat_server(int port_number){
int udp_sock = init_udpserver(port_number); //UDPサーバー初期化.
int tcp_sock = init_tcpserver(port_number, 5);  //TCPサーバー初期化.
int sock_accept;  //TCP接続用.
int sock_detect;  //selectにより検出されたソケット番号を格納.
fd_set readfds, mask;
struct sockaddr_in from_adrs;
socklen_t from_len;
int strsize;
User_info *temp;  //探索時などに使用.作業用.
int check; //クライアントが正常かどうか.

Max_sd = max(udp_sock, tcp_sock); //Max_sd=udp_sockとtcp_sockの大きい方.
FD_ZERO(&mask);
FD_SET(udp_sock, &mask);  //UDP監視用ビットマスクの設定.
FD_SET(tcp_sock, &mask);  //TCP監視用ビットマスクの設定.
FD_SET(0, &mask);  //サーバーによる直接入力監視用ビットマスクの設定.

/* このループがサーバーのメインの処理. */
while(1){
  memset(r_buf,'\0',BUFSIZE);
  readfds = mask;
  select( Max_sd+1, &readfds, NULL, NULL, NULL );

  /* UDPソケット監視. */
  if( FD_ISSET(udp_sock, &readfds) ){
    from_len = sizeof(from_adrs);
    Recvfrom(udp_sock, r_buf, BUFSIZE-1, 0, (struct sockaddr *)&from_adrs, &from_len); //UDPを通じてメッセージ受信.
    goto Branch;
  }

  /* 待ち受け用TCPソケット監視. */
  if( FD_ISSET(tcp_sock, &readfds) ){
    sock_accept = Accept(tcp_sock, NULL, NULL);  //接続を待ち受ける.
    FD_SET(sock_accept, &mask);  //新しいsock番号をビットマスクにセット.
    Max_sd = max(Max_sd, sock_accept);  //Max_sd < sock_acceptなら更新.
    append_node(sock_accept);  //ノードを追加する(usernameはまだ不明)
    continue;
  }

  /* 接続済みTCPソケット監視. */
  temp = &server;
  temp = temp->next;
  while(1){
    if( FD_ISSET(temp->sock, &readfds) ){
      sock_detect = temp->sock; //検出したソケット番号を保存.
      check = Recv(sock_detect, r_buf, BUFSIZE-1, 0);
      if(check == 0){
        exit_errmesg("client error");
      }
      goto Branch;
    }
    if(temp->next == NULL){
      break;
    }
    else{
      temp = temp->next;
    }
  }

  /* サーバーによる直接入力監視. */
  if( FD_ISSET(0, &readfds) ){
  memset(s_buf,'\0',BUFSIZE);
  fgets(s_buf, BUFSIZE, stdin);  // キーボードから文字列を入力する.
  chop_nl(s_buf);
  printf("server_mesg=%s\n", s_buf);
  create_message(0);  //[server] message 形式のメッセージ
  create_packet(MESSAGE, s_buf);  //ヘッダー追加
  send_to_others(0);  //サーバー以外(全てのクライアント)に送信.
  }

continue; //通信がなければ以下のBranchには到達しない.

Branch:
  packet = (struct idobata_packet *)r_buf; /* packetがバッファの先頭を指すようにする */
  chop_nl(packet->data);

  switch( analyze_header(packet->header) ){ /* ヘッダに応じて分岐 */

    case HELLO:
    HELLO_process(udp_sock, (struct sockaddr *)&from_adrs, sizeof(from_adrs));
    break;

    case JOIN:
    JOIN_process(sock_detect);  //線形リストのノードにusername情報を追加する.
    break;

    case POST:
    POST_process(sock_detect);  //「POST [username] message」形式の文をほかのすべてのクライアントに送信.
    break;

    case QUIT:
    QUIT_process(sock_detect);  //「[username]が退出しました.」というメッセージを送信し,ノードを消す.
  }
}

}

static int max(int a,int b){
  if(a < b){
    return b;
  }
  else{
    return a;
  }
}

static char *chop_nl(char *s)   //改行文字を消す
{
  int len;
  len = strlen(s);
  if( s[len-1] == '\n' ){
    s[len-1] = '\0';
  }
  return(s);
}

/* 発言者を特定し,[username] messageの形式でパケットを作成し,文字列長を返す. */
void create_message(int sock_detect){
  char copy[BUFSIZE];
  strcpy(copy, s_buf);
  memset(s_buf,'\0',BUFSIZE);
  chop_nl(copy); /* messageに改行があれば除く */
  User_info *temp = &server;
  while(1){
    if(temp->sock == sock_detect){
      sprintf(s_buf, "[%s] %s", temp->username, copy);
      break;
    }
    if(temp->next == NULL){
      break;
    }
    else{
      temp = temp->next;
    }
  }
}

static void send_to_others(int my_sock){
  User_info *temp;
  temp = &server;
  while(1){
    if(temp->sock != my_sock && temp->sock != 0){
//       Send(temp->sock, s_buf, strlen(s_buf),  MSG_NOSIGNAL);
      Send(temp->sock, s_buf, strlen(s_buf),  SO_NOSIGPIPE);

    }
    if(temp->next == NULL){
      break;
    }
    else{
      temp = temp->next;
    }
  }
}


/* リストの先頭にノードを追加 */
static void append_node(int sock_accept){
  User_info *newNode = (User_info*)malloc(sizeof(User_info));
  newNode->sock = sock_accept;
  newNode->next = server.next;  //先頭に追加.
  server.next = newNode;  //先頭を示すポインタの付け替え.
}

void HELLO_process(int udp_sock, const struct sockaddr *to, socklen_t tolen){
  create_packet(HERE, s_buf);
  Sendto(udp_sock, s_buf, strlen(s_buf), 0, to, tolen);
}

void JOIN_process(int sock_detect){
  chop_nl( packet->data ); /* Usernameに改行があれば除く */
  User_info *temp;
  for(temp=server.next; temp != NULL; temp=temp->next){
    if(temp->sock == sock_detect){
      strcpy(temp->username, packet->data); //usernameメンバに格納.
      return;
    }
  }
}

void POST_process(int sock_detect){
  memset(s_buf,'\0',BUFSIZE);
  strcpy(s_buf, packet->data);
  printf("%s\n",s_buf);
  create_message(sock_detect);  //[username] message 形式の文字列を作成.
  create_packet(MESSAGE, s_buf);  //MESG ヘッダーを上で作成した文字列に追加.
  send_to_others(sock_detect);  //発言者以外に送信.
}

void QUIT_process(int sock_detect){
  User_info *delNode, *prev;
  delNode = &server, prev = &server;
  delNode = delNode->next;
  while(1){
    if(delNode->sock == sock_detect){
    sprintf(s_buf, "[%s]が退出しました.", delNode->username);
    create_message(0);
    create_packet(MESSAGE, s_buf);
    send_to_others(0);
    prev->next = delNode->next;
    free(delNode);
    }
    if(server.next == NULL){
      exit_errmesg("There are no clients\n");
    }
    if(delNode->next == NULL){
      break;
    }
    delNode = delNode->next, prev = prev->next;
  }
}
