/**********************************************************************
 *  UDPサンプルサーバ (udps.c)
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

#define BUFSIZE      32768
#define MSGSIZE      1024
#define DEFAULT_PORT 5320

/* ルーティングテーブルを表示させるコマンド */
#ifdef __linux
#define COMMAND_ROUTE "/bin/netstat -rn"
#else
#define COMMAND_ROUTE "netstat -rn"
#endif

/* ARPテーブルを表示させるコマンド */
#ifdef __linux
#define COMMAND_ARP "/sbin/arp -an"
#else
#define COMMAND_ARP "arp -an"
#endif

/* TCPコネクションテーブルを表示させるコマンド */
#ifdef __linux
#define COMMAND_TCP "/bin/netstat -tn"
#else
#define COMMAND_TCP "netstat -n"
#endif

/* NICの情報を表示させるコマンド */
#ifdef __linux
#define COMMAND_NIC "/sbin/ifconfig -a"
#else
#define COMMAND_NIC "ifconfig -a"
#endif

enum {CMD_NAME, DST_PORT};

int execute(char *command, char *buf, int bufmax);

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* サーバのアドレス           */
  struct sockaddr_in client;  /* クライアントのアドレス     */
  unsigned int len;           /* sockaddr_inの長さ          */
  int port;                   /* サーバのポート番号         */
  int s;                      /* ソケットディスクリプタ     */
  int cn;                     /* 受信したコマンドのワード数 */
  int sn;                     /* 送信メッセージのバイト数   */
  int rn;                     /* 受信メッセージのバイト数   */
  int i;                      /* ループ変数                 */
  char cmd1[BUFSIZE];         /* 1ワード目のコマンド        */
  char cmd2[BUFSIZE];         /* 2ワード目のコマンド        */
  char recv_buf[BUFSIZE];     /* 受信バッファ               */
  char send_buf[BUFSIZE];     /* 送信バッファ               */

  /* 引き数の処理(ポート番号) */
  if (argc == 2) {
    if ((port = atoi(argv[DST_PORT])) == 0) {
      struct servent *se;     /* サービス情報 */

      if ((se = getservbyname(argv[DST_PORT], "udp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* UDPでソケットをオープンする */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* サーバのアドレスを設定する */
  memset(&server, 0, sizeof server);
  server.sin_family        = AF_INET;
  server.sin_addr.s_addr   = htonl(INADDR_ANY);
  server.sin_port          = htons(port);
  if (bind(s, (struct sockaddr *) &server, sizeof server) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* サーバ処理メインルーチン */
  while ((rn = recvfrom(s, recv_buf, BUFSIZE - 1, 0,
                        (struct sockaddr *) &client,
                        (len = sizeof client, &len))) >= 0) {
    recv_buf[rn] = '\0';
    printf("receive from '%s'\n", inet_ntoa(client.sin_addr));
    printf("receive data '%s'\n", recv_buf);

    /* 受信コマンドの処理 */
    if ((cn = sscanf(recv_buf, "%s%s", cmd1, cmd2)) <= 0)
      sn = 0; /* continue; */
    else if (cn == 2 && strcmp(cmd1, "show") == 0) {
      if (strcmp(cmd2, "route") == 0)
        sn = execute(COMMAND_ROUTE, send_buf, BUFSIZE);
      else if (strcmp(cmd2, "arp") == 0)
        sn = execute(COMMAND_ARP, send_buf, BUFSIZE);
      else if (strcmp(cmd2, "tcp") == 0)
        sn = execute(COMMAND_TCP, send_buf, BUFSIZE);
      else if (strcmp(cmd2, "nic") == 0)
        sn = execute(COMMAND_NIC, send_buf, BUFSIZE);
      else
        sn = snprintf(send_buf, BUFSIZE, "parameter error '%s'\n"
                                         "show [route|arp|tcp|nic]\n", cmd2);
    } else {
      if (strcmp(cmd1, "quit") == 0)
        break;

      send_buf[0] = '\0';
      if (cn != 1 && strcmp(cmd1, "help") != 0)
        snprintf(send_buf, BUFSIZE, "command error '%s'\n", cmd1);
      strncat(send_buf, "Command:\n"
                        "  show route\n"
                        "  show arp\n"
                        "  show tcp\n"
                        "  show nic\n"
                        "  quit\n"
                        "  help\n", BUFSIZE - strlen(send_buf) - 1);
      sn = strlen(send_buf);
    }

    /* メッセージ送信ループ (最後に必ずMSGSIZE未満のパケットを送信する) */
    for (i = 0; i <= sn; i += MSGSIZE) {
      int size;  /* 送信するメッセージのサイズ */

      if (sn - i > MSGSIZE)
        size = MSGSIZE;
      else if (sn - i <= 0)
        size = 0;
      else
        size = sn - i;
      if (sendto(s,&send_buf[i],size,0,(struct sockaddr *)&client,len) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }
    }
    printf("%s", send_buf);
  }
  close(s);

  return 0;
}

/*
 * int execute(char *command, char *buf, int bufmax);
 *
 * 機能
 *     コマンドを実行し、出力結果をバッファに格納する
 * 引き数
 *     char *command;  実行するコマンド
 *     char *buf;      出力結果を格納するバッファ
 *     int bufmax;     バッファの大きさ
 * 戻り値
 *     int             バッファに格納した文字数
 */
int execute(char *command, char *buf, int bufmax)
{
  FILE *fp;  /* ファイルポインタ         */
  int i;     /* 入力したデータのバイト数 */

  if ((fp = popen(command, "r")) == NULL) {
    perror(command);
    i = snprintf(buf, BUFSIZE, "server error: '%s' cannot execute.\n", command);
  } else {
    i = 0;
    while (i < bufmax - 1 && fscanf(fp, "%c", &buf[i]) == 1) 
      i++;
    buf[i] = '\0';
    pclose(fp);
  }
  return i;
}
