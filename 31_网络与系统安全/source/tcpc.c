/*****************************************************************
 *  TCPサンプルクライアントプログラム (tcpc.c)
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
 ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE      8192
#define DEFAULT_PORT 5320

enum {CMD_NAME, DST_IP, DST_PORT};

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* サーバのアドレス             */
  unsigned long dst_ip;       /* サーバのIPアドレス           */
  int port;                   /* ポート番号                   */
  int s;                      /* ソケットディスクリプタ       */
  int n;                      /* 入力データのバイト数         */
  int len;                    /* アプリケーションデータの長さ */
  char send_buf[BUFSIZE];     /* 送信バッファ                 */

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

      if ((se = getservbyname(argv[DST_PORT], "tcp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* TCPでソケットを開く */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* サーバのアドレスを設定しコネクションを確立する */
  memset(&server, 0, sizeof server);
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = dst_ip;
  server.sin_port        = htons(port);
  if (connect(s, (struct sockaddr *) &server, sizeof server) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to '%s'\n", inet_ntoa(server.sin_addr));

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

      /* ストリーム型メッセージの受信処理 */
      for (i = 0; i < BUFSIZE - 1; i++) {
        if (recv(s, &recv_buf[i], 1, 0) <= 0)
          goto exit_loop;
        if (recv_buf[i] == '\n')  /* 改行単位で受信処理をする */
          break;
      }
      if (i >= 1 && recv_buf[i - 1] == '\r') /* 改行がCR、LFの時の処理 */
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

  return 0;
}
