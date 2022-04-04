/**********************************************************************
 *  TCPサンプルクライアントプログラム (tcpv6c.c)
 *        Ver 2.0 2004年 7月 10日
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
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE      8192
#define DEFAULT_PORT "5320"

enum {CMD_NAME, DST_IP, DST_PORT};

int main(int argc, char *argv[])
{
  struct addrinfo info;    /* 接続先の情報                 */
  struct addrinfo *res0;   /* アドレスリストの先頭ポインタ */
  struct addrinfo *res;    /* アドレスリスト               */
  char ip[NI_MAXHOST];     /* IPアドレスを文字列として格納 */
  int s;                   /* ソケットディスクリプタ       */
  int n;                   /* 入力データのバイト数         */
  int len;                 /* アプリケーションデータの長さ */
  char send_buf[BUFSIZE];  /* 送信バッファ                 */
  int error;               /* getaddrinfoのエラーコード    */

  /* 引き数のチェック */
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s  hostname  [port]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* IPアドレス、ポート番号の設定(DNSへの問い合わせ) */
  memset(&info, 0, sizeof info);
  info.ai_family   = AF_UNSPEC;
  info.ai_socktype = SOCK_STREAM;
  error = getaddrinfo(argv[DST_IP], (argc == 3) ? argv[DST_PORT] : DEFAULT_PORT,
                      &info, &res0);
  if (error) {
    fprintf(stderr, "'%s' %s\n", argv[DST_IP], gai_strerror(error));
    exit(EXIT_FAILURE);
  }

  /* コネクションの確立ループ */
  for (res = res0; res; res = res->ai_next) {
    /* ソケットを開く */
    s = -1;
    if ((s = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) >= 0) {
      /* 接続するサーバのアドレスとプロトコルの種類の表示 */
      if (getnameinfo((struct sockaddr *) res->ai_addr, res->ai_addrlen, ip,
                      sizeof ip, NULL, 0, NI_NUMERICHOST)) {
        fprintf(stderr, "getnameinfo error\n");
        exit(EXIT_FAILURE);
      }
      printf("connecting '%s' ", ip);
      if (res->ai_addr->sa_family == AF_INET6)
        printf("by IPv6. ");
      else if (res->ai_addr->sa_family == AF_INET)
        printf("by IPv4. ");
      else
        printf("by unkown protocol. ");
      fflush(stdout);

      /* コネクション確立要求開始 */
      if (connect(s, res->ai_addr, res->ai_addrlen) >= 0) {
        printf("connected.\n");
        break;
      } else {
        printf("error.\n");
        close(s);
      }
    }
  }
  if (res == NULL) {
    printf("\n");
    fprintf(stderr, "socket or connect error\n");
    exit(EXIT_FAILURE);
  }

  /* 
   * クライアント処理メインルーチン
   */
  while (1) {
    char cmd[BUFSIZE];       /* 入力コマンドの検査用 */
    char recv_buf[BUFSIZE];  /* 受信バッファ         */

    /*
     * コマンド入力処理・送信処理
     */
    printf("TCP>");
    fflush(stdout);

    /* コマンドの入力 */
    if (fgets(send_buf, BUFSIZE - 2, stdin) == NULL)
      break;
    cmd[0] = '\0';
    sscanf(send_buf, "%s", cmd);
    if (strcmp(cmd, "quit") == 0)
      break;
    if (strcmp(cmd, "") == 0)
      strcpy(send_buf, "help\n");

#ifdef HTTP
    strncat(send_buf, "Connection: keep-alive\n\n", BUFSIZE
                                       - strlen(send_buf) - 1);
#endif

    /* コマンドの送信 */
    if (send(s, send_buf, strlen(send_buf), 0) <= 0) {
      perror("send");
      break;
    }

    /*
     * アプリケーションヘッダの受信・解析
     */
    len = -1;
    while (1) {
      char *cmd1;  /* コマンドの1ワード目 */
      char *cmd2;  /* コマンドの2ワード目 */
      int i;       /* 受信文字のカウント  */

      /* ストリーム型のデータの受信処理 */
      for (i = 0; i < BUFSIZE - 1; i++) {
        if (recv(s, &recv_buf[i], 1, 0) <= 0)
          goto exit_loop;
        if (recv_buf[i] == '\n')  /* 改行単位で受信処理をする */
          break;
      }
      if (i >= 1 && recv_buf[i - 1] == '\r')  /* 改行がCR、LFの時の処理 */
        i--;
      if (i == 0)  /* 空行の場合は、ヘッダが終わり、データ部が始まる */
        break;

      /* ヘッダの解析処理 */
      recv_buf[i] = '\0';
      cmd1 = strtok(recv_buf, ": ");
      cmd2 = strtok(NULL, " \0");
#ifdef DEBUG
      printf("[%s, %s]\n", cmd1, cmd2);
#endif
      if (strcmp("Content-Length", cmd1) == 0)
        len = atoi(cmd2);
    }

#ifdef HTTP
    if (len == -1) {
      while ((n = recv(s, recv_buf, BUFSIZE - 1, 0)) > 0) {
        recv_buf[n] = '\0';
        printf("%s", recv_buf);
        fflush(stdout);
      }
      close(s);
      return 0;
    }
#endif

    /*
     * アプリケーションデータの受信、画面への出力
     */
    while (len > 0) {
      if ((n = recv(s, recv_buf, BUFSIZE - 1, 0)) <= 0)
        goto exit_loop;
      recv_buf[n] = '\0';
      len -= n;
      printf("%s", recv_buf);
      fflush(stdout);
    }
  }
 exit_loop:
  n = snprintf(send_buf, BUFSIZE, "quit\n");
  send(s, send_buf, n, 0);
  close(s);
  freeaddrinfo(res0);
  printf("connection closed.\n");

  return 0;
}
