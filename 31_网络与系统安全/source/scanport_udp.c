/**********************************************************************
 *  UDPポートスキャンプログラム (scanport_udp.c)
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
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#define  __FAVOR_BSD
#include <netinet/udp.h>

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

#define MAXBUFF 8192

enum {CMD_NAME, DST_IP, START_PORT, LAST_PORT};
enum {CONNECT, NOCONNECT};

int udpportscan(u_int32_t dst_ip, int dst_port, int send_sd, int recv_sd);

int main(int argc, char *argv[])
{
  int recv_sd;       /* 受信用ディスクリプタ                      */
  int send_sd;       /* 送信用ディスクリプタ                      */
  u_int32_t dst_ip;  /* 終点IPアドレス (ネットワークバイトオーダ) */
  int end_port;      /* スキャン開始ポート番号                    */
  int start_port;    /* スキャン終了ポート番号                    */
  int dst_port;      /* スキャンするポート番号                    */

  /* コマンドライン引き数の数のチェック */
  if (argc != 4) {
    fprintf(stderr, "usage: %s dst_ip start_port last_port\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* コマンドライン引き数から値をセットする */
  dst_ip     = inet_addr(argv[DST_IP]);
  start_port = atoi(argv[START_PORT]);
  end_port   = atoi(argv[LAST_PORT]);

  CHKADDRESS(dst_ip);

  /* 送信用UDPソケットのオープン */
  if ((send_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket(SOCK_DGRAM)");
    exit(EXIT_FAILURE);
  }

  /* 受信用rawソケットのオープン */
  if ((recv_sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }

  /* スキャンポートメインループ */
  for (dst_port = start_port; dst_port <= end_port; dst_port++) {
    printf("Scan Port %d\r", dst_port);
    fflush(stdout);

    if (udpportscan(dst_ip, dst_port, send_sd, recv_sd) == CONNECT) {
      /* selectがタイムアウトしたとき */
      struct servent *se;  /* サービス情報 */

      se = getservbyport(htons(dst_port), "udp");
      printf("%5d %-20s\n", dst_port, (se==NULL) ? "unknown" : se->s_name);
    }
    usleep(10 * 1000);
  }
  close(send_sd);
  close(recv_sd);

  return 0;
}

/*
 * int udpportscan(u_int32_t dst_ip, int dst_port)
 * 機能
 *     指定したIPアドレス、ポート番号にUDPで通信できるか調べる。
 * 引き数 
 *     u_int32_t dst_ip;  送信先IPアドレス (ネットワークバイトオーダ)
 *     int dst_port;      送信先ポート番号
 * 戻り値
 *     int                CONNECT  ... ポートが開いている
 *                        NOCONNECT .. ポートが閉まっている
 */
int udpportscan(u_int32_t dst_ip, int dst_port, int send_sd, int recv_sd)
{
  struct sockaddr_in send_sa;  /* 送信先のアドレス情報      */
  char buff[MAXBUFF];          /* パケット受信用バッファ    */
  struct timeval tv;           /* selectのタイムアウト時間  */
  fd_set select_fd;            /* select用ビットマップ      */

  /* 送信先アドレスの設定 */
  memset(&send_sa, 0, sizeof send_sa);
  send_sa.sin_family      = AF_INET;
  send_sa.sin_addr.s_addr = dst_ip;
  send_sa.sin_port        = htons(dst_port);

  /* UDPパケットの送信 (ヘッダのみ送信、データは0バイト) */
  sendto(send_sd, NULL, 0, 0, (struct sockaddr *) &send_sa, sizeof send_sa);

  /* selectのタイムアウトの設定 */
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  while (1) {
    /* selectで検査するディスクリプタの設定 */
    FD_ZERO(&select_fd);
    FD_SET(recv_sd, &select_fd);

    if (select(recv_sd + 1, &select_fd, NULL, NULL, &tv) > 0) {
      struct icmp *icmp;   /* icmpヘッダ                    */
      struct ip *ip;       /* IPヘッダ構造体                */
      int ihlen;           /* IPヘッダの長さ                */
      struct ip *ip2;      /* ICMPデータ中のIPヘッダ構造体  */
      int ihlen2;          /* ICMPデータ中のIPヘッダの長さ  */
      struct udphdr *udp;  /* ポート番号                    */

      /* ICMPパケットの受信 (56 = 20(IP) + 8(ICMP) + 20(IP) + 8(UDP)) */
      if (recvfrom(recv_sd, buff, MAXBUFF, 0, NULL, NULL) < 56)
        continue;

      ip     = (struct ip *) buff;
      ihlen  = ip->ip_hl << 2;
      icmp   = (struct icmp *) ((char *) ip + ihlen);
      ip2    = (struct ip *) icmp->icmp_data;
      ihlen2 = ip2->ip_hl << 2;
      udp    = (struct udphdr *) ((char *) ip2 + ihlen2);

      /* 受信したパケットがポート到達不能ならwhileループを抜ける */
      if ((ip->ip_src.s_addr == send_sa.sin_addr.s_addr)
          && (icmp->icmp_type == ICMP_UNREACH)
          && (icmp->icmp_code == ICMP_UNREACH_PORT)
          && (ip2->ip_p == IPPROTO_UDP)
          && (udp->uh_dport == send_sa.sin_port))
        break;
    } else
      return CONNECT;
  }
  return NOCONNECT;
}
