/**********************************************************************
 *  ホストスキャンプログラム (scanhost.c)
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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
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

#define BUFSIZE    4096
#define PACKET_LEN 72

enum {CMD_NAME, START_IP, LAST_IP};

void make_icmp8_packet(struct icmp *icmp, int len, int n);
void tvsub(struct timeval *out, struct timeval *in);
u_int16_t checksum(u_int16_t *data, int len);

int main(int argc, char *argv[])
{
  struct sockaddr_in send_sa;  /* 送信先のアドレス                */
  int s;                       /* ソケットディスクリプタ          */
  char send_buff[PACKET_LEN];  /* 送信バッファ                    */
  char recv_buff[BUFSIZE];     /* 受信バッファ                    */
  int start_ip;                /* スキャンするIPアドレスの開始値  */
  int end_ip;                  /* スキャンするIPアドレスの終了値  */
  int dst_ip;                  /* スキャンするIPアドレスの値      */
  int on = 1;                  /* ON                              */

  if (argc != 3) {
    fprintf(stderr, "usage: %s start_ip last_ip\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* スキャンするIPアドレスの範囲を設定 */
  start_ip = ntohl(inet_addr(argv[START_IP]));
  end_ip   = ntohl(inet_addr(argv[LAST_IP]));

  memset(&send_sa, 0, sizeof send_sa);
  send_sa.sin_family = AF_INET;

  /* ICMP/IP送受信用rawソケットのオープン */
  if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW, IPPROTO_ICMP)");
    exit(EXIT_FAILURE);
  }
  /* BROADCASTパケットも送信できるようにする */
  if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof on) < 0) {
    perror("setsockopt(SOL_SOCKET, SO_BROADCAST)");
    exit(EXIT_FAILURE);
  }

  /*
   * スキャンホストメインルーチン
   */
  for (dst_ip = start_ip; dst_ip <= end_ip; dst_ip++) {
    int i;  /* ループ変数 */

    send_sa.sin_addr.s_addr = htonl(dst_ip);
    CHKADDRESS(send_sa.sin_addr);

    for (i = 0; i < 3; i++) {
      struct timeval tv;  /* 時刻の情報 */

      printf("scan %s (%d)\r", inet_ntoa(send_sa.sin_addr), i + 1);
      fflush(stdout);

      /* ICMPエコー要求パケット(送信時刻入り)を作成して送信 */
      make_icmp8_packet((struct icmp *) send_buff, PACKET_LEN, i);
      if (sendto(s, (char *) &send_buff, PACKET_LEN, 0,
                 (struct sockaddr *) &send_sa, sizeof send_sa) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }

      /* selectのタイムアウトの設定 */
      tv.tv_sec  = 0;
      tv.tv_usec = 200 * 1000;

      while (1) {
        fd_set readfd;  /* selectするファイルディスクリプタ */
        struct ip *ip;  /* ipヘッダ構造体   */
        int ihlen;      /* ヘッダ長   */

        /* selectでパケットの到着を待つ */
        FD_ZERO(&readfd);
        FD_SET(s, &readfd);
        if (select(s + 1, &readfd, NULL, NULL, &tv) <= 0)
          break;

        /* パケット受信 */
        if (recvfrom(s, recv_buff, BUFSIZE, 0, NULL, NULL) < 0) {
          perror("recvfrom");
          exit(EXIT_FAILURE);
        }

        ip = (struct ip *) recv_buff;
        ihlen = ip->ip_hl << 2;
        if (ip->ip_src.s_addr == send_sa.sin_addr.s_addr) {
          struct icmp *icmp;  /* icmpパケット構造体 */

          icmp = (struct icmp *) (recv_buff + ihlen);
          if (icmp->icmp_type == ICMP_ECHOREPLY) {
            /* ICMPエコー応答を返してきたIPアドレスを表示 */
            printf("%-15s", inet_ntoa(*(struct in_addr *)
                                      &(ip->ip_src.s_addr)));
            /* 往復時間(RTT)の計算と表示 */
            gettimeofday(&tv, (struct timezone *) 0);
            tvsub(&tv, (struct timeval *) (icmp->icmp_data));
            printf(": RTT = %8.4f ms\n", tv.tv_sec*1000.0 + tv.tv_usec/1000.0);
            goto exit_loop;
          }
        }
      }
    }
 exit_loop: ;
  }
  close(s);

  return 0;
}

/*
 * void make_icmp8_packet(struct icmp *icmp, int len, int n);
 * 機能
 *     ICMPパケットの作成
 * 引き数 
 *     struct icmp *icmp;  作成するICMPヘッダの先頭アドレス
 *     int len;            ICMPエコー要求のパケット長
 *     int n;              ICMPエコー要求のシーケンス番号
 * 戻り値
 *     なし
 */
void make_icmp8_packet(struct icmp *icmp, int len, int n)
{
  memset(icmp, 0, len);

  /* 時刻情報をデータ部に記録。ネットワークバイトオーダーを気にしない */
  gettimeofday((struct timeval *) (icmp->icmp_data), (struct timezone *) 0);

  /* ICMPヘッダの作成 */
  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_id   = 0;  /* 本来IDにはプロセスIDなどの識別子を格納する */
  icmp->icmp_seq  = n;  /* ネットワークバイトオーダーを気にしない */

  /* チェックサムの計算 */
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = checksum((u_int16_t *) icmp, len);
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

/*
 * u_int16_t checksum(u_int16_t *data, int len);
 * 機能
 *     チェックサムの計算
 * 引き数 
 *     u_int16_t *data;  チェックサムを求めるデータ
 *     int len;          データのバイト数
 * 戻り値
 *     u_int16_t         チェックサムの値 (補数値)
 */
u_int16_t checksum(u_int16_t *data, int len)
{
  u_int32_t sum = 0; /* 求めるチェックサム */

  /* 2バイトずつ加算 */
  for (; len > 1; len -= 2) {
    sum += *data++;
    if (sum & 0x80000000) 
      sum = (sum & 0xffff) + (sum >> 16);
  }

  /* データ長が奇数バイトの場合の処理 */
  if (len == 1) {
    u_int16_t i = 0;
    *(u_char*) (&i) = *(u_char *) data;
    sum += i;
  }

  /* 桁あふれを折り返す */
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  return ~sum;
}
