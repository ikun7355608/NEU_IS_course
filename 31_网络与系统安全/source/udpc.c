/**********************************************************************
 *  UDPサンプルクライアント (udpc.c)
 *        Ver 2.1 2007年 3月 6日
 *                                制作・著作 村山公保 (Yukio Murayama)
 *
 *  使用許諾書
 *    本プログラムは、TCP/IPプロトコルの学習、及び、ネットワークプロ
 *    グラミングの技能を向上させるためにのみ、そのまま、または、修正
 *    して使用することができます。本プログラムについて、法律で禁止さ
 *    れているか、または、公序良俗に反するような改造、及び、使用を禁
 *    止します。本プログラムは無保証です。制作者は本プログラムによっ
 *    て発生したいかなる損害についても責任を取ることはできません。
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>

#define MSGSIZE      1024
#define BUFSIZE      (MSGSIZE + 1)
#define DEFAULT_PORT 5320

enum {CMD_NAME, DST_IP, DST_PORT};

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* サーバのアドレス        */
  unsigned long dst_ip;       /* サーバのIPアドレス      */
  int port;                   /* ポート番号              */
  int s;                      /* ソケットディスクリプタ  */
  unsigned int zero;          /* ゼロ                    */

  /* 引き数のチェック */
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s  hostname  [port]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* サーバのIPアドレスを調べる */
  if ((dst_ip = inet_addr(argv[DST_IP])) == INADDR_NONE) {
    struct hostent *he;  /* ホスト情報 */

    if ((he = gethostbyname(argv[DST_IP])) == NULL) {
      fprintf(stderr, "gethostbyname error\n");
      exit(EXIT_FAILURE);
    }
    memcpy((char *) &dst_ip, (char *) he->h_addr, he->h_length);
  }

  /* サーバのポート番号を調べる */
  if (argc == 3) {
    if ((port = atoi(argv[DST_PORT])) == 0) {
      struct servent *se;  /* サービス情報 */

      if ((se = getservbyname(argv[DST_PORT], "udp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* UDPでソケットを開く */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* サーバのアドレスを設定する */
  memset(&server, 0, sizeof server);
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = dst_ip;
  server.sin_port        = htons(port);

  /*
   * クライアント処理メインルーチン
   */
  zero = 0;
  printf("UDP>");
  fflush(stdout);

  while (1) {
    char buf[BUFSIZE];  /* 受信バッファ                   */
    char cmd[BUFSIZE];  /* 送信バッファ                   */
    int n;              /* 入力データのバイト数           */
    struct timeval tv;  /* selectのタイムアウト時間       */
    fd_set readfd;      /* selectで検出するディスクリプタ */

    /* selectのタイムアウトの設定 */
    tv.tv_sec  = 600;
    tv.tv_usec = 0;

    /* selectによるイベント待ち */
    FD_ZERO(&readfd);
    FD_SET(0, &readfd);  /* 標準入力 */
    FD_SET(s, &readfd);  /* サーバ   */
    if ((select(s + 1, &readfd, NULL, NULL, &tv)) <= 0) {
      fprintf(stderr, "\nTimeout\n");
      break;
    }

    /* 標準入力からの入力コマンドの処理 */
    if (FD_ISSET(0, &readfd)) {
      if ((n = read(0, buf, BUFSIZE - 1)) <= 0)
        break;
      buf[n] = '\0';
      sscanf(buf, "%s", cmd);
      if (strcmp(cmd, "quit") == 0)
        break;
      if (sendto(s, buf, n, 0, (struct sockaddr *) &server, sizeof server) < 0)
        break;
    }

    /* サーバからの応答メッセージの処理 */
    if (FD_ISSET(s, &readfd)) {
      if ((n = recvfrom(s, buf, MSGSIZE, 0, (struct sockaddr *) 0, &zero)) < 0)
        break;
      buf[n] = '\0';
      printf("%s", buf);
      if (n < MSGSIZE)
        printf("UDP>");
      fflush(stdout);
    }
  }
  close(s);

  return 0;
}
