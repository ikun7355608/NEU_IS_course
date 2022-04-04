/**********************************************************************
 *  ARPテーブル更新プログラム (arpupdate.c)
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
#include <sys/socket.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <netinet/in_systm.h>
#ifndef __linux
#include <sys/ioctl.h>
#include <net/bpf.h>
#include <net/if.h>
#include <fcntl.h>
#endif
#include <netinet/if_ether.h>

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

#define MAXSIZE 8192
#define CMAX    256

enum {CMD_NAME, IFNAME, DST_IP, MAC_ADDR, OPTION};
enum {NORMAL, REPLY, REQUEST};

#ifndef __linux
int open_bpf(char *ifname, int *bufsize);
#endif

void make_ethernet(struct ether_header *eth, unsigned char *ether_dhost,
                   unsigned char *ether_shost, u_int16_t ether_type);
void make_arp(struct ether_arp *arp, int op, unsigned char *arp_sha,
              unsigned char *arp_spa, unsigned char *arp_tha,
              unsigned char *arp_tpa);
void print_ethernet(struct ether_header *eth);
void print_arp(struct ether_arp *arp);
char *mac_ntoa(unsigned char d[]);
void help(char *cmd);

int main(int argc, char *argv[])
{
  int s;               /* ソケットディスクリプタ          */
  u_char mac_addr[6];  /* MACアドレス格納用               */
  int tmp[6];          /* MACアドレス格納用(一時利用)     */
  int flag;            /* フラグ (REPLY、REQUEST、NORMAL) */
  int i;               /* ループ変数                      */
  u_int32_t dst_ip;    /* ターゲットIPアドレス            */
  char ifname[CMAX];   /* インタフェース名                */
#ifndef __linux
  struct bpf_hdr *bp;  /* BPFヘッダ構造体                 */
  int bpf_len;         /* BPFでの受信データの長さ         */
  int bufsize;         /* BPF内部のバッファサイズ         */
#else
  struct sockaddr sa;  /* ソケットアドレス構造体          */
#endif

  flag = NORMAL;
  if (argc == 5) {
    if (strcmp(argv[OPTION], "reply") == 0) 
      flag = REPLY;
    else if (strcmp(argv[OPTION], "request") == 0)
      flag = REQUEST;
    else {
      help(argv[CMD_NAME]);
      exit(EXIT_FAILURE);
    }
  } else if (argc != 4) {
    help(argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  snprintf(ifname, CMAX, "%s", argv[IFNAME]);
  dst_ip = inet_addr(argv[DST_IP]);

  if (sscanf(argv[MAC_ADDR], "%x:%x:%x:%x:%x:%x", &tmp[0], &tmp[1],
             &tmp[2], &tmp[3], &tmp[4], &tmp[5]) != 6) {
    printf("MAC address error %s\n", argv[MAC_ADDR]);
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < 6; i++)
    mac_addr[i] = (char) tmp[i];

  CHKADDRESS(dst_ip);

#ifdef __linux
  if ((s = socket(PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&sa, 0, sizeof sa);
  sa.sa_family = PF_PACKET;
  snprintf(sa.sa_data, CMAX, "%s", ifname);

  if (bind(s, &sa, sizeof sa) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
#else
  if ((s = open_bpf(ifname, &bufsize)) < 0)
    exit(EXIT_FAILURE);
  bpf_len = 0;
#endif

  while (1) {
    struct ether_header *eth;  /* Ethernetヘッダ構造体                      */
    char recv_buff[MAXSIZE];   /* 受信バッファ                              */
    char send_buff[MAXSIZE];   /* 送信バッファ                              */
    char *rp;                  /* ヘッダの先頭を表す作業用ポインタ(受信用)  */
    char *rp0;                 /* パケットの先頭を表すポインタ(受信用)      */
    char *sp;                  /* ヘッダの先頭を表す作業用ポインタ(送信用)  */
    int len;                   /* 各種の長さ(一時利用)                      */

#ifndef __linux
    if (bpf_len <= 0) {
      if ((bpf_len = read(s, recv_buff, bufsize)) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      bp = (struct bpf_hdr *) recv_buff;
    } else {
      bp = (struct bpf_hdr *) ((char *) bp + bp->bh_hdrlen + bp->bh_caplen);
      bp = (struct bpf_hdr *) BPF_WORDALIGN((int) bp);
    }
    rp = rp0 = (char *) bp + bp->bh_hdrlen;
    len = bp->bh_caplen;
#ifdef DEBUG
    printf("bpf_len=%d,",  bpf_len);
    printf("hdrlen=%d,",   bp->bh_hdrlen);
    printf("caplen=%d,",   bp->bh_caplen);
    printf("datalen=%d\n", bp->bh_datalen);
#endif
    bpf_len -= BPF_WORDALIGN(bp->bh_hdrlen + bp->bh_caplen);
#else
    if ((len = read(s, recv_buff, MAXSIZE)) < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    rp = rp0 = recv_buff;
#endif
    eth = (struct ether_header *) rp;
    rp = rp + sizeof (struct ether_header);

    /* arpupdateが送信したパケット及びその返事は無視する */
    if ( memcmp(eth->ether_dhost, mac_addr, 6) != 0 
      && memcmp(eth->ether_shost, mac_addr, 6) != 0
      && ntohs(eth->ether_type) == ETHERTYPE_ARP) {
      struct ether_arp *arp;  /* ARPパケット構造体 */

      arp = (struct ether_arp *) rp;

      if (dst_ip == *(int *) (arp->arp_spa)) {
        static u_char zero[6]; /* Ethernet NULL MACアドレス */
        static u_char one[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
                               /* EthernetブロードキャストMACアドレス */

        printf("Hit****************\n");
        print_ethernet(eth);
        print_arp(arp);

        sp = send_buff + sizeof (struct ether_header);

        switch (flag) {
          case REPLY:
            make_arp((struct ether_arp *) sp, ARPOP_REPLY, mac_addr, 
                     arp->arp_tpa, arp->arp_sha, arp->arp_spa);
            make_ethernet((struct ether_header *) send_buff, arp->arp_sha,
                          mac_addr, ETHERTYPE_ARP);
            break;
          case REQUEST:
            make_arp((struct ether_arp *) sp, ARPOP_REQUEST, mac_addr,
                     arp->arp_spa, zero, arp->arp_tpa);
            make_ethernet((struct ether_header *) send_buff, one, mac_addr,
                          ETHERTYPE_ARP);
            break;
          default:
            make_arp((struct ether_arp *) sp, ARPOP_REQUEST, mac_addr,
                     arp->arp_tpa, zero, arp->arp_spa);
            make_ethernet((struct ether_header *) send_buff,
                          arp->arp_sha, mac_addr, ETHERTYPE_ARP);
            break;
        }
        len = sizeof (struct ether_header) + sizeof (struct ether_arp);

        /* パケット受信後、0.5秒待ってからパケットを送信 */
        usleep(500 * 1000);
#ifndef __linux
        if (write(s, send_buff, len) < 0) {
          perror("write");
          exit(EXIT_FAILURE);
        }
#else
        if (sendto(s, send_buff, len, 0, &sa, sizeof sa) < 0) {
          perror("sendto");
          exit(EXIT_FAILURE);
        }
#endif
        printf("SEND---------------------------\n");
        print_ethernet((struct ether_header *) send_buff);
        print_arp((struct ether_arp *) sp);
      }
    }
  }
  return 0;
}

/*
 * void make_ethernet(struct ether_header *eth, unsigned char *ether_dhost,
 *                    unsigned char *ether_shost, u_int16_t ether_type)
 * 機能
 *     Ethernetヘッダの作成
 * 引き数 
 *     struct ether_header *eth;    作成するヘッダEthernetヘッダ
 *     unsigend char *ether_dhost;  終点MACアドレス
 *     unsigend char *ether_shost;  始点MACアドレス
 *     u_int16_t ether_type;        タイプ
 * 戻り値
 *     なし
 */
void make_ethernet(struct ether_header *eth, unsigned char *ether_dhost,
                   unsigned char *ether_shost, u_int16_t ether_type)
{
  memcpy(eth->ether_dhost, ether_dhost, 6);
  memcpy(eth->ether_shost, ether_shost, 6);
  eth->ether_type = htons(ether_type);
}

/*
 * void make_arp(struct ether_arp *arp, int op, unsigned char *arp_sha,
 *               unsigned char *arp_spa, unsigned char *arp_tha,
 *               unsigned char *arp_tpa);
 * 機能
 *     ARPパケットの作成
 * 引き数 
 *     struct ether_arp *arp;   作成するARPヘッダ
 *     int op;                  オペレーション
 *     unsigned char *arp_sha;  始点MACアドレス
 *     unsigned *arp_spa;       始点IPアドレス
 *     unsigned *arp_tha;       ターゲットMACアドレス
 *     unsigned *arp_tpa;       ターゲットIPアドレス
 * 戻り値
 *     なし
 */
void make_arp(struct ether_arp *arp, int op, unsigned char *arp_sha,
              unsigned char *arp_spa, unsigned char *arp_tha,
              unsigned char *arp_tpa)
{
  arp->arp_hrd  = htons(1);
  arp->arp_pro  = htons(ETHERTYPE_IP);
  arp->arp_hln  = 6;
  arp->arp_pln  = 4;
  arp->arp_op   = htons(op);
  memcpy(arp->arp_sha, arp_sha, 6);
  memcpy(arp->arp_spa, arp_spa, 4);
  memcpy(arp->arp_tha, arp_tha, 6);
  memcpy(arp->arp_tpa, arp_tpa, 4);
}

/*
 * char *mac_ntoa(unsigned char *d);
 * 機能
 *    配列に格納されているMACアドレスを文字列に変換
 *    static変数を利用するため、非リエントラント関数
 * 引き数
 *    unsigned char *d;  MACアドレスが格納されている領域の先頭アドレス
 * 戻り値
 *    文字列に変換されたMACアドレス
 */
char *mac_ntoa(unsigned char *d)
{
#define MACSTRLEN 50
  static char str[MACSTRLEN];  /* 文字列に変換したMACアドレスを保存 */

  snprintf(str, MACSTRLEN, "%02x:%02x:%02x:%02x:%02x:%02x",
           d[0], d[1], d[2], d[3], d[4], d[5]);

  return str;
}

/*
 * void print_ethernet(struct ether_header *eth);
 * 機能
 *     Ethernetヘッダの表示
 * 引き数
 *     struct ether_header *eth;  Ethernetヘッダ構造体へのポインタ
 * 戻り値
 *     なし
 */
void print_ethernet(struct ether_header *eth)
{
  int type = ntohs(eth->ether_type); /* Ethernetタイプ */

  if (type <= 1500)
    printf("IEEE 802.3 Ethernet Frame:\n");
  else 
    printf("Ethernet Frame:\n");

  printf("+-------------------------+-------------------------"
         "+-------------------------+\n");
  printf("| Destination MAC Address:                          "
         "         %17s|\n", mac_ntoa(eth->ether_dhost));
  printf("+-------------------------+-------------------------"
         "+-------------------------+\n");
  printf("| Source MAC Address:                               "
         "         %17s|\n", mac_ntoa(eth->ether_shost));
  printf("+-------------------------+-------------------------"
         "+-------------------------+\n");
  if (type < 1500)
    printf("| Length:            %5u|\n", type);
  else 
    printf("| Ethernet Typ:     0x%04x|\n", type);
  printf("+-------------------------+\n");
}

/*
 * void print_arp(struct ether_arp *arp);
 * 機能
 *     ARPパケットの表示
 * 引き数
 *     struct ether_arp *arp;  ARPパケット構造体へのポインタ
 * 戻り値
 *     なし
 */
void print_arp(struct ether_arp *arp)
{
  static char *arp_op_name[] = {
    "Undefine",
    "(ARP Request)",
    "(ARP Reply)",
    "(RARP Request)",
    "(RARP Reply)"
  };   /* オペレーションの種類を表す文字列 */
#define ARP_OP_MAX (sizeof arp_op_name / sizeof arp_op_name[0])
  int op = ntohs(arp->ea_hdr.ar_op);  /* ARPオペレーション */

  if (op <= 0 || ARP_OP_MAX < op)
    op = 0;

  printf("Protocol: ARP\n");
  printf("+-------------------------+-------------------------+\n");
  printf("| Hard Type: %2u%-11s| Protocol:0x%04x%-9s|\n",
         ntohs(arp->ea_hdr.ar_hrd),
         (ntohs(arp->ea_hdr.ar_hrd)==ARPHRD_ETHER)?"(Ethernet)":"(Not Ether)",
         ntohs(arp->ea_hdr.ar_pro),
         (ntohs(arp->ea_hdr.ar_pro)==ETHERTYPE_IP)?"(IP)":"(Not IP)");
  printf("+------------+------------+-------------------------+\n");
  printf("| HardLen:%3u| Addr Len:%2u| OP: %4d%16s|\n",
         arp->ea_hdr.ar_hln, arp->ea_hdr.ar_pln, ntohs(arp->ea_hdr.ar_op),
         arp_op_name[op]);
  printf("+------------+------------+-------------------------"
         "+-------------------------+\n");
  printf("| Source MAC Address:                               "
         "         %17s|\n", mac_ntoa(arp->arp_sha));
  printf("+---------------------------------------------------"
         "+-------------------------+\n");
  printf("| Source IP Address:                 %15s|\n",
         inet_ntoa(*(struct in_addr *) &arp->arp_spa));
  printf("+---------------------------------------------------"
         "+-------------------------+\n");
  printf("| Destination MAC Address:                          "
         "         %17s|\n", mac_ntoa(arp->arp_tha));
  printf("+---------------------------------------------------"
         "+-------------------------+\n");
  printf("| Destination IP Address:            %15s|\n",
         inet_ntoa(*(struct in_addr *) &arp->arp_tpa));
  printf("+---------------------------------------------------+\n");
}

/*
 * void help(char *cmd);
 * 機能
 *     arpupdateの使用方法の表示
 * 引き数
 *     char *cmd;    コマンド名 (arpupdate)
 * 戻り値
 *     なし
 */
void help(char *cmd)
{
  fprintf(stderr, "usage: %s ifname dst_ip mac_addr [reply|request]\n", cmd);
}

#ifndef __linux
/*
 * int open_bpf(char *ifname, int *bufsize);
 * 機能
 *     BPFをオープンする
 * 引き数
 *     char *ifname;  インタフェース名
 *     int *bufsize;  BPF内部のバッファサイズ
 * 戻り値
 *     int            ファイルディスクリプタ
 */
int open_bpf(char *ifname, int *bufsize)
{
  char buf[CMAX];    /* 文字列格納用  */
  int bpfd;          /* ファイルディスクリプタ  */
  struct ifreq ifr;  /* インタフェース属性構造体  */
  int i;             /* ループ変数    */

  /* BPFデバイスファイルのオープン */
  for (i = 0; i < 4; i++) { 
    snprintf(buf, CMAX, "/dev/bpf%d", i);
    if ((bpfd = open(buf, O_RDWR, 0)) > 0)
      break;
  }

  if (i >= 5) {
    fprintf(stderr, "cannot open BPF\n");
    return -1;
  }

  /* BPF内部のバッファサイズの設定 */
  *bufsize = MAXSIZE;
  if (ioctl(bpfd, BIOCSBLEN, bufsize) < 0) {
    snprintf(buf, CMAX, "ioctl(BIOCSBLEN, %d)", *bufsize);
    perror(buf);
    return -1;
  }

  /* インタフェース名の設定 */
  snprintf(ifr.ifr_name, CMAX, "%s", ifname);
  if (ioctl(bpfd, BIOCSETIF, &ifr) < 0) {
    snprintf(buf, CMAX, "ioctl(BIOCSETIF, '%s')", ifname);
    perror(buf);
    return -1;
  }
  fprintf(stderr, "BPF read from '%s' (%s)\n", ifr.ifr_name, buf);

  /* promiscuousモード */
  if (ioctl(bpfd, BIOCPROMISC, NULL) < 0) {
    perror("ioctl(BIOCPROMISC)");
    return -1;
  }

  /* 即時モード */
  i = 1;
  if (ioctl(bpfd, BIOCIMMEDIATE, &i) < 0) {
    perror("ioctl(BIOCIMMEDIATE)");
    return -1;
  }

  return bpfd;
}
#endif
