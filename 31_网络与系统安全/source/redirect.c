/**********************************************************************
 *  ICMPリダイレクトプログラム (redirect.c)
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#define __FAVOR_BSD
#include <netinet/udp.h>
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

enum {CMD_NAME, TARGET_IP, OLD_ROUTER, NEW_ROUTER, DST_IP};

void make_udp_header(struct udphdr *udp);
void make_ip_header(struct ip *ip, int target_ip, int dst_ip, int proto,
                    int iplen);
void make_icmp5_header(struct icmp *icmp, u_int32_t gw_ip);
u_int16_t checksum(u_int16_t *data, int len);

int  main(int argc, char *argv[])
{
  struct sockaddr_in dest;   /* 終点ホストのアドレス            */
  unsigned char buff[1500];  /* 送信バッファ                    */
  struct ip *ip_new;         /* IPヘッダへのポインタ            */
  struct ip *ip_old;         /* IPヘッダへのポインタ            */
  struct icmp *icmp;         /* ICMPヘッダへのポインタ          */
  struct udphdr *udp;        /* UDPヘッダへのポインタ           */
  int s;                     /* ソケットファイルディスクリプタ  */
  int size;                  /* 各種バイト数                    */
  int on = 1;                /* ON                              */

  /* コマンドライン引き数のチェック */
  if (argc != 5) {
    fprintf(stderr, "usage %s targetd_host old_router new_router dst_ip\n",
            argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* rawソケットのオープン */
  if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }
  /* IPヘッダを自前で作成する */
  if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof on) < 0) {
    perror("setsockopt(IP_HDRINCL)");
    exit(EXIT_FAILURE);
  }

  /* 各ヘッダのポインタの設定 */
  /* IP(20) + ICMP(8) + IP(20) + UDP(8) */
  ip_new = (struct ip     *) (buff);
  icmp   = (struct icmp   *) (buff + 20);
  ip_old = (struct ip     *) (buff + 20 + 8);
  udp    = (struct udphdr *) (buff + 20 + 8 + 20);
  size   = 20 + 8 + 20 + 8;

  /* パケットの作成 */
  make_udp_header(udp);
  make_ip_header(ip_old, inet_addr(argv[TARGET_IP]), inet_addr(argv[DST_IP]),
                 IPPROTO_UDP, 100);
  make_icmp5_header(icmp, inet_addr(argv[NEW_ROUTER]));
  make_ip_header(ip_new, inet_addr(argv[OLD_ROUTER]),
                 inet_addr(argv[TARGET_IP]), IPPROTO_ICMP, size);

  /* 送信アドレスの設定 */
  memset(&dest, 0, sizeof dest);
  dest.sin_family      = AF_INET;
  dest.sin_addr.s_addr = inet_addr(argv[TARGET_IP]);
  CHKADDRESS(dest.sin_addr.s_addr);

  /* パケットの送信 */
  if (sendto(s, buff, size, 0, (struct sockaddr *) &dest, sizeof dest) < 0) {
    perror("sendto");
    exit(EXIT_FAILURE);
  }

  close(s);

  return 0;
}

/*
 * void make_ip_header(struct ip *ip, int target_ip, int dst_ip, int proto,
 *                     int iplen)
 * 機能
 *     IPヘッダの作成
 * 引き数
 *     struct ip *ip;  作成するIPヘッダの先頭アドレス
 *     int target_ip;  送信元IPアドレス
 *     int dst_ip;     送信先IPアドレス
 *     int proto;      プロトコル
 *     int iplen;      IPデータグラムの全長
 * 戻り値
 *     なし
 */
void make_ip_header(struct ip *ip, int target_ip, int dst_ip, int proto,
                    int iplen)
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
  ip->ip_off = htons(IP_DF);
#else
  /* BSDのraw IPの場合 */
  ip->ip_len = iplen;
  ip->ip_off = IP_DF;
#endif
  ip->ip_ttl = 2;
  ip->ip_p   = proto;
  ip->ip_src.s_addr = target_ip;
  ip->ip_dst.s_addr = dst_ip;

  /* チェックサムの計算 (raw ipはヘッダチェックサムを計算してくれるが、
     ICMPデータ部のIPヘッダまでは計算してくれない) */
  ip->ip_sum = 0;
  ip->ip_sum = checksum((u_int16_t *) ip, sizeof ip);
}

/*
 * void make_icmp5_header(struct icmp *icmp, u_int32_t gw_ip);
 * 機能
 *     ICMP タイプ5(リダイレクト)ヘッダの作成
 * 引き数
 *     struct icmp *icmp;  作成するICMPヘッダの先頭アドレス
 *     int n;              ICMPエコー要求のシーケンス番号
 * 戻り値
 *     なし
 */
void make_icmp5_header(struct icmp *icmp, u_int32_t gw_ip)
{
  icmp->icmp_type  = ICMP_REDIRECT;
  icmp->icmp_code  = ICMP_REDIRECT_HOST;
  icmp->icmp_gwaddr.s_addr = gw_ip;
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = checksum((u_int16_t *) icmp, 8 + 20 + 8);
  /* ICMP (8) + IP (20) + UDP (8) */
}

/*
 * void make_udp_header(struct udphdr *udp);
 * 機能
 *     UDPヘッダの作成。各フィールドの値はダミー。
 * 引き数
 *     struct udphdr *udp;  作成するUDPヘッダの先頭アドレス
 * 戻り値
 *     なし
 */
void make_udp_header(struct udphdr *udp)
{
  udp->uh_sport = htons(2000);
  udp->uh_ulen  = htons(72);
  udp->uh_dport = htons(33434);
  udp->uh_sum   = htons(9999);
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
