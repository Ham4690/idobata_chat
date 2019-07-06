#include <mynet.h>

#define PORT 50000         /* ポート番号 */
#define S_BUFSIZE 100   /* 送信用バッファサイズ */
#define R_BUFSIZE 100
int main(){

  int sock;
  char servername[] = "localhost";
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;

  sock = init_tcpclient(servername,PORT);

  return 0;
}
