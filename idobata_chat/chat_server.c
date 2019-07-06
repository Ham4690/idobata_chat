/*
  chat_server.c
*/
  #include "chat.h"
#include "mynet.h"
#include <stdlib.h>
#include <unistd.h>

void chat_server(int port_number, int n_client)
{
  int sock_listen;

  /* サーバの初期化 */
// sock_listen : 待ち受けようのソケット
  sock_listen = init_tcpserver(port_number,5);

  /* クライアントの接続 */
  init_client(sock_listen, n_client);
//接続が終わったので待ち受け用のソケット
//sock_listenをcloseする
  close(sock_listen);

  /* メインループ */
  //実際のサーバの処理内容
  communication_loop();

}