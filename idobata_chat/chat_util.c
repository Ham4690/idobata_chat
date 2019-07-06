/*
作成にあたって工夫:
・各クライアントがそれぞれexitと入力すれば終了できるようにした
・サーバーが何かの原因で終了した際にクライアント側に"server error"
というメッセージが出るようにした.
・自分のその他のクライアントでメッセージの表示を区別し自分の発言と
その他をみてわかりやすくした。
・3人以上でも通信できるようにした。

苦労した点
プログラムが大きくなってきたのでどの部分が関係し合っているのか理解
することが難しかった。
リンクのエラーや定義されていないと行ったエラーの修正箇所を見つけるのが
大変だった。

起動時のコマンド
server:
~$./chat -S -p 50000 -c <繋ぎたい人数>
 ex:
  ~$./chat -S -p 50000 -c 3

client:
~$./chat -C -s localhost -p 50000

*/


/*
  chat_util.c
*/
#include "mynet.h"
#include "chat.h"
#include <stdlib.h>
#include <sys/select.h>

#define NAMELENGTH 20 /* ログイン名の長さ制限 */
#define BUFLEN 500    /* 通信バッファサイズ */

/* 各クライアントの:w
ユーザ情報を格納する構造体の定義 */
typedef struct{
  int  sock;
  char name[NAMELENGTH];
} client_info;

/* プライベート変数 */
static int N_client;         /* クライアントの数 */
static client_info *Client;  /* クライアントの情報 */
static int Max_sd;               /* ディスクリプタ最大値 */
static char Buf[BUFLEN];     /* 通信用バッファ */

/* プライベート関数 */

static int client_login(int sock_listen);
static void recv_msg();
static void start_chat();
static void client_delete();
static char *chop_nl(char *s);

void init_client(int sock_listen, int n_client)
{
  N_client = n_client;

  /* クライアント情報の保存用構造体の初期化 */
  if( (Client=(client_info *)malloc(N_client*sizeof(client_info)))==NULL ){
    exit_errmesg("malloc()");
  }

  

  /* クライアントのログイン処理 */
  Max_sd = client_login(sock_listen);

}

void communication_loop()
{
  char *question;

  for(;;){
    start_chat();

    recv_msg();
  }
}

static int client_login(int sock_listen)
{
  int client_id,sock_accepted;
  static char prompt[]="Input your name: ";
  char loginname[NAMELENGTH];
  int strsize;

  for( client_id=0; client_id<N_client; client_id++){
    /* クライアントの接続を受け付ける */
    sock_accepted = Accept(sock_listen, NULL, NULL);
    printf("Client[%d] connected.\n",client_id);

    /* ログインプロンプトを送信 */
    Send(sock_accepted, prompt, strlen(prompt), 0);

    /* ログイン名を受信 */
    strsize = Recv(sock_accepted, loginname, NAMELENGTH-1, 0);
    loginname[strsize] = '\0';
    chop_nl(loginname);

    /* ユーザ情報を保存 */
    Client[client_id].sock = sock_accepted;
    strncpy(Client[client_id].name, loginname, NAMELENGTH);
  }
//接続する人数分for文を回すので自動的に
// sock_acceptedは最後の人のsock(最大)となる
  return(sock_accepted);

}

/*

開始時に接続したクライアントの名前,
また終了したい際にはexitと入力するという説明を全体へ送信する

例:Taro,Yamamoto,Hanakoの3名がチャットを行う場合

:Taro側(Taro,Yamamoto,Hanako共通のため他の二人は省略)

Connected.
Input your name: Taro
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------

*/

static void start_chat(){
  int client_id;
  char send_msg[100];
  char add_msg[30];
  sprintf(send_msg,"--------------------------------------------------\n");
  for(client_id=0; client_id<N_client; client_id++){
    Send( Client[client_id].sock, send_msg, strlen(send_msg),0);
  }
  sprintf(send_msg,"login:%s",Client[0].name);
  for(client_id = 1; client_id<N_client; client_id++){
    sprintf(add_msg,",%s",Client[client_id].name);
    strcat(send_msg,add_msg);
  }
  for(client_id=0; client_id<N_client; client_id++){
    Send( Client[client_id].sock, send_msg, strlen(send_msg),0);
  }

  
  sprintf(send_msg,"\nIf you want to end chat:exit\n");
  for(client_id=0; client_id<N_client; client_id++){
    Send( Client[client_id].sock, send_msg, strlen(send_msg),0);
  }
  sprintf(send_msg,"--------------------------------------------------\n");
  for(client_id=0; client_id<N_client; client_id++){
    Send( Client[client_id].sock, send_msg, strlen(send_msg),0);
  }
}

/*
サーバへ送られてきたメッセージを全体へ送信する

送ってきたクライアント番号と送ってきた内容を受け取る。
送ってきた相手の画面には入力したメッセージが表示される
ので全体へ送ると送信者のみ同じメッセージが2つ表示されて
します。そのため送信者以外のクライアントへ送信を行なって
いる。
また、自分の送信したメッセージと他のクライアントが送ってきた
メッセージを区別できるよう送信する際にはスペースによって
見分けがつくようにしている

例:Taro,Yamamoto,Hanakoの3名がチャットを行う場合

:Taro側
Connected.
Input your name: Taro
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
                             [ Hanako ]:hello
Hi
                             [ Yamamoto ]:good morning

:Yamamoto側
Connected.
Input your name: Yamamoto
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
                             [ Hanako ]:hello
                             [ Taro ]:Hi
good morning

:Hanako側
Connected.
Input your name: Hanako
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
hello
                             [ Taro ]:Hi
                             [ Yamamoto ]:good morning

 */
static void send_msg(char *msg, int client_number){

  char send_msg[100];
  int client_id;
  sprintf(send_msg,"                             [ %s ]:",Client[client_number].name);
  strcat(send_msg,msg);
  for(client_id=0; client_id<N_client; client_id++){
    if(client_id != client_number){
      Send( Client[client_id].sock, send_msg, strlen(send_msg),0);
    }
  }

}

/*
クライアントからの受信確認

各クライアントからの送られてきたメッセージがあるかどうか、
また、クライアントが強制終了または終了した場合はFINパケット
を受け取った場合とexit\nと入力された場合で条件分岐を行い
強制終了の場合は各クライアントにlogoutと送信して終了するようにしている.
終了(exitと入力)の場合は各クライアントにexitとlogoutを送信して終了するようにしている.

例1:Taro,Yamamoto,Hanakoの3名がチャットを行いYamamotoが終了(exit)する場合

:Taro側
Connected.
Input your name: Taro
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
Hi
                             [ Yamamoto ]:Hello
                             [ Hanako ]:Hello
                             [ Yamamoto ]:exit
                             [ Yamamoto ]:logout
Hi
                             [ Hanako ]:Hi


:Yamamoto側
Connected.
Input your name: Yamamoto
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
                             [ Taro ]:Hi
Hello
                             [ Hanako ]:Hello
exit

:Hanako側
Connected.
Input your name: Hanako
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
                             [ Taro ]:Hi
                             [ Yamamoto ]:Hello
Hello
                             [ Yamamoto ]:exit
                             [ Yamamoto ]:logout


例2:Taro,Yamamoto,Hanakoの3名がチャットを行いYamamotoが強制終了(control + C)する場合

:Taro側
Connected.
Input your name: Taro
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
Hi
                             [ Yamamoto ]:Hello
                             [ Hanako ]:Hello
                             [ Yamamoto ]:logout
Hi
                             [ Hanako ]:Hi

:Yamamoto側
Connected.
Input your name: Yamamoto
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
                             [ Taro ]:Hi
Hello
                             [ Hanako ]:Hello
^C


:Hanako側
Connected.
Input your name: Hanako
--------------------------------------------------
login:Taro,Yamamoto,Hanako
If you want to end chat:exit
--------------------------------------------------
                             [ Taro ]:Hi
                             [ Yamamoto ]:Hello
Hello
                             [ Yamamoto ]:logout
                             [ Taro ]:Hi
Hi


 */

static void recv_msg()
{
  fd_set mask, readfds;
  int client_id;
  int answered;
  int strsize;
  int i;
  static char exit_msg[] = "logout\n";
  int id_test;

  /* ビットマスクの準備 */
  FD_ZERO(&mask);
  for(client_id=0; client_id<N_client; client_id++){
      FD_SET(Client[client_id].sock,&mask);
  }


  while(1){

    /* 受信データの有無をチェック */
    readfds = mask;
    select( Max_sd+1, &readfds, NULL, NULL, NULL );

    for( client_id=0; client_id<N_client; client_id++ ){

      if( FD_ISSET(Client[client_id].sock, &readfds) ){

        strsize = Recv(Client[client_id].sock, Buf, BUFLEN-1,0);
        if( strsize == 0 ){
          send_msg(exit_msg,client_id);
          client_delete(mask,client_id);
        }else{
          Buf[strsize]='\0';
          send_msg(Buf,client_id);
        }
        memset(Buf,0,sizeof(BUFLEN));

      }
    }
  }
}

static char *chop_nl(char *s)
{
  int len;
  len = strlen(s);
  if( s[len-1] == '\n' ){
    s[len-1] = '\0';
  }
  return(s);
}

/*
終了クライアントの仕分け作業

クライアンが終了(exitと入力)、または強制終了(control + Cなど
でFINパッケージを受け取った時)した場合にそのクライアントの
sock位置に設定していたmaskの値を0に変更しチェックしないように設定
その後終了クライアントの位置以下の全てのクライアントを一つずつ繰り
上げて全体数(N_client)を一つ減らし、全体へ送信する際などに終了した
分のクライアントへ送信しないようにしている.
*/
static void client_delete(fd_set mask,int client_id){
  int i;
  FD_CLR(Client[client_id].sock,&mask);
  if(client_id != N_client-1){
    for(i=client_id;i<N_client-1;i++){
      Client[i].sock = Client[i+1].sock;
      strcpy(Client[i].name,Client[i+1].name);
    }
  }
  N_client--;
}