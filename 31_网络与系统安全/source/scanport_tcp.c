/**********************************************************************
 *  TCPポートスキャンプログラム (scanport_tcp.c)
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define CHKADDRESS(_saddr_) \
        {\
          unsigned char *p = (unsigned char *) &(_saddr_);\
          if ((p[0] == 127)\
           || (p[0] == 10)\
           || (p[0] == 172 && 16 <= p[1] && p[1] <= 31)\
           || (p[0] == 192 && p[1] == 168))\
            ;\
          else {\
            fprintf(stderr, "IP address error.\n");\
            exit(EXIT_FAILURE);\
          }\
        }

enum {CMD_NAME, DST_IP, START_PORT, LAST_PORT};
enum {CONNECT, NOCONNECT};

int tcpportscan(u_int32_t dst_ip, int dst_port);

int main(int argc, char *argv[]) 
{
  u_int32_t dst_ip;  /* 終点IPアドレス          */
  int dst_port;      /* 終点ポート番号          */
  int start_port;    /* スキャン開始ポート番号  */
  int end_port;      /* スキャン終了ポート番号  */

  /* コマンドライン引き数のチェック */
  if (argc != 4) {
    fprintf(stderr, "usage: %s dst_ip start_port last_port\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* コマンドライン引き数から値をセットする */
  dst_ip     = inet_addr(argv[DST_IP]);
  start_port = atoi(argv[START_PORT]);
  end_port   = atoi(argv[LAST_PORT]);

  CHKADDRESS(dst_ip);

  /* メインルーチン */
  for (dst_port = start_port; dst_port <= end_port; dst_port++) {
    printf("Scan Port %d\r", dst_port);
    fflush(stdout);

    if (tcpportscan(dst_ip, dst_port) == CONNECT) {
      struct servent *se;  /* サービス情報 */

      se = getservbyport(htons(dst_port), "tcp");
      printf("%5d %-20s\n", dst_port, (se == NULL) ? "unknown" : se->s_name);
    }
  }

  return 0;
}

/*
 * int tcpportscan(u_int32_t dst_ip, int dst_port)
 * 機能
 *     指定したIPアドレス、ポート番号にTCPコネクションが確立できるか調べる。
 * 引き数 
 *     u_int32_t dst_ip;  送信先IPアドレス (ネットワークバイトオーダー)
 *     int dst_port;      送信先ポート番号
 * 戻り値
 *     int                CONNECT  ... コネクションが確立できた
 *                        NOCONNECT .. コネクションが確立できなかった
 */
int tcpportscan(u_int32_t dst_ip, int dst_port)
{
  struct sockaddr_in dest;  /* ソケットアドレス       */
  int s;                    /* ソケットディスクリプタ */
  int ret;                  /* この関数の戻り値       */

  /* 終点アドレスの設定 */
  memset(&dest, 0, sizeof dest);
  dest.sin_family      = AF_INET;
  dest.sin_port        = htons(dst_port);
  dest.sin_addr.s_addr = dst_ip;

  /* TCPソケットのオープン */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* TCPコネクション確立要求 */
  if (connect(s, (struct sockaddr *) &dest, sizeof dest) < 0)
    ret = NOCONNECT;
  else
    ret = CONNECT;
  close(s);

  return ret;
}
