/**********************************************************************
 *  ルートスキャンプログラム (scanroute.c)
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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#define __FAVOR_BSD
#include <netinet/udp.h>
#include <arpa/inet.h>

#define BUFSIZE 512

enum {CMD_NAME, DST_IP};
enum {ON, OFF};

struct packet_udp {
  struct ip ip;
  struct udphdr udp;
};

void make_ip_header(struct ip *ip, int srcip, int dstip, int iplen);
void make_udp_header(struct udphdr *udp);
void tvsub(struct timeval *out, struct timeval *in);

int main(int argc, char *argv[])
{
  struct packet_udp sendpacket;  /* 送信するUDP/IPパケット          */
  struct sockaddr_in send_sa;    /* 終点のアドレス                  */
  int send_sd;                   /* 送信用ソケットディスクリプタ    */
  int recv_sd;                   /* 受信用ソケットディスクリプタ    */
  int len;                       /* 送信するIPパケット長            */
  int ttl;                       /* TTLの値                         */
  u_char buff[BUFSIZE];          /* 受信バッファ                    */
  struct icmp *icmp;             /* ICMP構造体                      */
  struct ip *ip;                 /* IP構造体                        */
  struct in_addr ipaddr;         /* IPアドレス構造体                */
  int on = 1;                    /* ON                              */
  int dns_flg = ON;              /* ホストのドメイン名を検索するか  */

  /* 引き数-nオプションのチェック */
  if (argc == 3 && strcmp(argv[1], "-n") == 0) {
    dns_flg = OFF;
    argv[1] = argv[2];
    argv[2] = NULL;
    argc = 2;
  }

  /* 引き数のチェック */
  if (argc != 2) {
    fprintf(stderr, "usage: %s [-n] dst_ip\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  memset(&send_sa, 0, sizeof send_sa);
  send_sa.sin_family = AF_INET;

  /* ドメイン名からIPアドレスへの変換 */
  if ((send_sa.sin_addr.s_addr = inet_addr(argv[DST_IP])) == INADDR_NONE) {
    struct hostent *he;  /* ホスト情報 */

    if ((he = gethostbyname(argv[DST_IP])) == NULL) {
      fprintf(stderr, "unknown host %s\n", argv[DST_IP]);
      exit(EXIT_FAILURE);
    }
    send_sa.sin_family = he->h_addrtype;
    memcpy((char *) &(send_sa.sin_addr), he->h_addr, sizeof he->h_length);
  }

  /* UDP/IP送信用rawソケットのオープン */
  if ((send_sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }
  /* IPヘッダを自前で作成する */
  if (setsockopt(send_sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof on) < 0) {
    perror("setsockopt(IPPROTO_IP, IP_HDRINCL)");
    exit(EXIT_FAILURE);
  }

  /* ICMP受信用rawソケットのオープン */
  if ((recv_sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }

  /* UDP/IPパケットの作成 */
  len = sizeof (struct packet_udp);
  memset(&sendpacket, 0, sizeof sendpacket);
  make_udp_header(&(sendpacket.udp));
  make_ip_header(&(sendpacket.ip), 0, send_sa.sin_addr.s_addr, len);

  /*
   * スキャンルートメインルーチン
   */
  printf("scanroute %s\n", inet_ntoa(send_sa.sin_addr));
  for (ttl = 1; ttl <= 64; ttl++) {
    struct timeval tvm0;  /* パケット送信時刻     */
    struct timeval tvm1;  /* パケット受信時刻     */
    struct timeval tv;    /* selectのタイムアウト */
    int i;                /* ループ変数           */

    printf("%2d: ", ttl);
    fflush(stdout);

    sendpacket.ip.ip_ttl = ttl;

    for (i = 0; i < 3; i++) {
      /* UDPパケットの送信 */
      if (sendto(send_sd, (char *) &sendpacket, len, 0,
                 (struct sockaddr *) &send_sa, sizeof send_sa) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }
      /* 送信時刻の記録 */
      gettimeofday(&tvm0, (struct timezone *) 0);

      /* selectのタイムアウトの設定 */
      tv.tv_sec  = 3;
      tv.tv_usec = 0;

      /* パケット受信ループ */
      while (1) {
        fd_set readfd;  /* selectで検査するディスクリプタ */
        int n;          /* selectの戻り値                 */
        int ihlen;      /* ヘッダ長                       */

        /* selectでパケットを受信するまで待つ */
        FD_ZERO(&readfd);
        FD_SET(recv_sd, &readfd);
        if ((n = select(recv_sd + 1, &readfd, NULL, NULL, &tv)) < 0) {
          perror("select");
          exit(EXIT_FAILURE);
        }
        if (n == 0) {
          printf("? ");
          fflush(stdout);
          break;
        }

        /* ICMPパケットの受信 */
        if (recvfrom(recv_sd, buff, BUFSIZE, 0, NULL, NULL) < 0) {
          perror("recvfrom");
          exit(EXIT_FAILURE);
        }
        /* パケット受信時刻の記録 */
        gettimeofday(&tvm1, (struct timezone *) 0);

        /* 関係あるパケットかどうかを検査 */
        ip = (struct ip *) buff;
        ihlen = ip->ip_hl << 2;
        icmp = (struct icmp *) (buff + ihlen);
        if ((icmp->icmp_type == ICMP_TIMXCEED
             && icmp->icmp_code == ICMP_TIMXCEED_INTRANS)
            || icmp->icmp_type == ICMP_UNREACH)
          goto exit_loop;
      }
    }
   exit_loop:
    if (i < 3) {
      struct hostent *host;          /* ホスト情報              */
      char hostip[INET_ADDRSTRLEN];  /* IPアドレスを表す文字列  */

      /* IPアドレスとドメイン名の表示 */
      memcpy(&ipaddr, &(ip->ip_src.s_addr), sizeof ipaddr);
      snprintf(hostip, INET_ADDRSTRLEN, "%s", inet_ntoa(*(struct in_addr *)
                                                        &(ip->ip_src.s_addr)));
      if (dns_flg == OFF)
        printf("%-15s", hostip);
      else if ((host = gethostbyaddr((char *) &ipaddr, 4, AF_INET)) == NULL)
        printf("%-15s (%s) ", hostip, hostip);
      else
        printf("%-15s (%s) ", hostip, host->h_name);

      /* 往復時間の表示 */
      tvsub(&tvm1, &tvm0);
      printf(": RTT =%8.4f ms", tvm1.tv_sec * 1000.0 + tvm1.tv_usec / 1000.0);

      /* 標的ホストに到着したかどうかの検査 */
      if (icmp->icmp_type == ICMP_UNREACH) {
        switch (icmp->icmp_code) {
          case ICMP_UNREACH_PORT:
            printf("  Reach!\n");
            break;
          case ICMP_UNREACH_HOST:
            printf("  Host Unreachable!\n");
            break;
          case ICMP_UNREACH_NET:
            printf("  Network Unreachable!\n");
            break;
        }
        goto end_program;
      }
    }
    printf("\n");
  }
 end_program:
  close(send_sd);
  close(recv_sd);

  return 0;
}

/*
 * void make_udp_header(struct udphdr *udp);
 * 機能
 *     UDPヘッダの作成
 * 引き数
 *     struct udphdr *udp;  作成するUDPヘッダの先頭アドレス
 * 戻り値
 *     なし
 */
void make_udp_header(struct udphdr *udp)
{
  udp->uh_sport = htons(33434);
  udp->uh_ulen  = htons((u_int16_t) sizeof (struct udphdr));
  udp->uh_dport = htons(33434);  /* tracerouteのポート番号 */
  udp->uh_sum   = htons(0);
}

/*
 * void make_ip_header(struct ip *ip, int srcip, int dstip, int iplen);
 * 機能
 *     IPヘッダの作成
 * 引き数
 *     struct ip *ip;  作成するIPヘッダの先頭アドレス
 *     int srcip;      送信元IPアドレス
 *     int dstip;      送信先IPアドレス
 *     int iplen;      IPデータグラムの全長
 * 戻り値
 *     なし
 */
void make_ip_header(struct ip *ip, int srcip, int dstip, int iplen)
{
  memset(ip, 0, sizeof ip);

  /* IPヘッダの作成 */
  ip->ip_v   = IPVERSION;
  ip->ip_hl  = sizeof (struct ip) >> 2;
  ip->ip_id  = htons(0);
  ip->ip_off = 0;

#ifdef __linux
  /* Linuxのraw IPの場合 */
  ip->ip_len = htons(iplen);
  ip->ip_off = htons(0);
#else
  /* BSDのraw IPの場合 */
  ip->ip_len = iplen;
  ip->ip_off = 0;
#endif
  ip->ip_ttl = 64;
  ip->ip_p   = IPPROTO_UDP;
  ip->ip_src.s_addr = srcip;
  ip->ip_dst.s_addr = dstip;

  /* チェックサムの計算 */
  ip->ip_sum = 0;
}

/*
 * void tvsub(struct timeval *out, struct timeval *in);
 * 機能
 *     strcut timevalの引き算。結果はoutに格納される。
 * 引き数 
 *     struct timeval *out;  引かれる数
 *     struct timeval *in;   引く数
 * 戻り値
 *     なし
 */
void tvsub(struct timeval *out, struct timeval *in)
{
  if ((out->tv_usec -= in->tv_usec) < 0) {
    out->tv_sec--;
    out->tv_usec += 1000000;
  }
  out->tv_sec -= in->tv_sec;
}
