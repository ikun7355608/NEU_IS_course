/**********************************************************************
 *  TCP/IP�ѥ��åȥ�˥���󥰥ץ���� (ipdump.c)
 *        Ver 2.1 2007ǯ 3�� 6��
 *                                ������� ¼������ (Yukio Murayama)
 *
 *  ���ѵ�����
 *    �ܥץ����ϡ�TCP/IP�ץ�ȥ���γؽ����ڤӡ��ͥåȥ���ץ�
 *    ����ߥ󥰤ε�ǽ����夵���뤿��ˤΤߡ����Τޤޡ��ޤ��ϡ�����
 *    ���ƻ��Ѥ��뤳�Ȥ��Ǥ��ޤ����ܥץ����ˤĤ��ơ�ˡΧ�Ƕػߤ�
 *    ��Ƥ��뤫���ޤ��ϡ�������¯��ȿ����褦�ʲ�¤���ڤӡ����Ѥ��
 *    �ߤ��ޤ����ܥץ�����̵�ݾڤǤ�������Ԥ��ܥץ����ˤ��
 *    ��ȯ�����������ʤ�»���ˤĤ��Ƥ���Ǥ���뤳�ȤϤǤ��ޤ���
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#define __FAVOR_BSD
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#ifndef __linux
#include <sys/ioctl.h>
#include <net/bpf.h>
#include <net/if.h>
#include <fcntl.h>
#endif
#include <netinet/if_ether.h>

#define MAXSIZE 8192
#define CMAX    256
#define OPTNUM  9
#define ON      1
#define OFF     0
#define DEF_IF  "en0"   /* Mac OS X�Υǥե���ȥ��󥿥ե�����̾ */

enum {ETHER, ARP, IP, TCP, UDP, ICMP, DUMP, ALL, TIME};
enum {IP_ADDR, PORT};

/* �ѥ��åȥե��륿�ѥޥ��� */
#define FILTER_ARP(_p) (filter.flg[IP_ADDR] ?\
                       ((*(int *) (_p)->arp_spa == filter.ip.s_addr\
                         || *(int *) (_p)->arp_tpa == filter.ip.s_addr) ?\
                        1 : 0)\
                       : 1)
#define FILTER_IP(_p) (filter.flg[IP_ADDR] ?\
                       (((_p)->ip_src.s_addr == filter.ip.s_addr\
                         || (_p)->ip_dst.s_addr == filter.ip.s_addr) ?\
                        1 : 0)\
                       : 1)
#define FILTER_TCP(_p) (filter.flg[PORT] ?\
                        (((_p)->th_sport == filter.port\
                          || (_p)->th_dport == filter.port) ?\
                         1 : 0)\
                        : 1)
#define FILTER_UDP(_p) (filter.flg[PORT] ?\
                        (((_p)->uh_sport == filter.port\
                          || (_p)->uh_dport == filter.port) ?\
                         1 : 0)\
                        : 1)

struct filter {
  struct in_addr ip;  /* IP���ɥ쥹     */
  u_int16_t port;     /* �ݡ����ֹ�     */
  int flg[2];         /* �ե��륿�ե饰 */
};

#ifndef __linux
int open_bpf(char *ifname, int *bufsize);
#endif

void print_ethernet(struct ether_header *eth);
void print_arp(struct ether_arp *arp);
void print_ip(struct ip *ip);
void print_icmp(struct icmp *icmp);
void print_tcp(struct tcphdr *tcp);
void print_tcp_mini(struct tcphdr *tcp);
void print_udp(struct udphdr *udp);
void dump_packet(unsigned char *buff, int len);
char *mac_ntoa(u_char *d);
char *tcp_ftoa(int flag);
char *ip_ttoa(int flag);
char *ip_ftoa(int flag);
void help(char *cmd);

int main(int argc, char **argv)
{
  int s;                   /* �����åȥǥ�������ץ�    */
  int c;                   /* getopt()�Ǽ�������ʸ��    */
  char ifname[CMAX] = "";  /* ���󥿥ե�����̾          */
  int opt[OPTNUM];         /* ɽ�����ץ����Υե饰    */
  extern int optind;       /* getopt()�Υ����Х��ѿ�  */
#ifndef __linux
  struct bpf_hdr *bp;      /* BPF�إå���¤��           */
  int bpf_len;             /* BPF�Ǥμ����ǡ�����Ĺ��   */
  int bufsize;             /* BPF�����ΥХåե�������   */
#endif 
  struct filter filter;    /* �ե��륿�������          */

  /* ɽ������ѥ��å�(���ץ����)�ν���� */
  opt[ETHER] = OFF;
  opt[ARP]   = ON;
  opt[IP]    = ON;
  opt[TCP]   = ON;
  opt[UDP]   = ON;
  opt[ICMP]  = ON;
  opt[DUMP]  = OFF;
  opt[ALL]   = OFF;
  opt[TIME]  = OFF;
  /* �ե��륿�ν���� */
  filter.flg[IP_ADDR] = OFF;
  filter.flg[PORT]    = OFF;

  /* ���ޥ�ɥ饤�󥪥ץ����θ��� */
  while ((c = getopt(argc, argv, "aei:p:f:dhft")) != EOF) {
    switch (c) {
      case 'a':  /* all */
        opt[ALL]  = ON;
        break;
      case 'e':  /* ethernet */
        opt[ETHER]= ON;
        break;
      case 'd':  /* dump */
        opt[DUMP] = ON;
        break;
      case 't':  /* time */
        opt[TIME] = ON;
        break;
      case 'i':  /* if name */
        snprintf(ifname, CMAX, "%s", optarg);
        break;
      case 'p':  /* protocol */
        opt[ARP]  = OFF;
        opt[IP]   = OFF;
        opt[TCP]  = OFF;
        opt[UDP]  = OFF;
        opt[ICMP] = OFF;
        optind--;
        while (argv[optind] != NULL && argv[optind][0] != '-') {
          if (strcmp(argv[optind], "arp") == 0)
            opt[ARP]  = ON;
          else if (strcmp(argv[optind], "ip") == 0)
            opt[IP]   = ON;
          else if (strcmp(argv[optind], "tcp") == 0)
            opt[TCP]  = ON;
          else if (strcmp(argv[optind], "udp") == 0)
            opt[UDP]  = ON;
          else if (strcmp(argv[optind], "icmp") == 0)
            opt[ICMP] = ON;
          else if (strcmp(argv[optind], "other") == 0)
            ;
          else {
            help(argv[0]);
            exit(EXIT_FAILURE);
          }
          optind++;
        }
        break;
      case 'f':  /* filter */
        optind--;
        while (argv[optind] != NULL && argv[optind][0] != '-') {
          if (strcmp(argv[optind], "ip") == 0 && argv[optind+1] != NULL) {
            filter.flg[IP_ADDR] = ON;
            filter.ip.s_addr = inet_addr(argv[++optind]);
          } else if (strcmp(argv[optind], "port")==0 && argv[optind+1]!=NULL) {
            filter.flg[PORT] = ON;
            filter.port = htons(atoi(argv[++optind]));
          } else {
            help(argv[0]);
            exit(EXIT_FAILURE);
          }
          optind++;
        }
        break;
      case 'h':  /* help */
      case '?':
      default:
        help(argv[0]);
        exit(EXIT_FAILURE);
        break;
    }
  }

  if (optind < argc) {
    while (optind < argc)
      printf("%s ", argv[optind++]);
    printf("\n");
    help(argv[0]);
    exit(EXIT_FAILURE);
  }

  if (filter.flg[IP_ADDR])
    printf("filter ip   = %s\n", inet_ntoa(filter.ip));
  if (filter.flg[PORT])
    printf("filter port = %d\n", htons(filter.port));

#ifdef __linux
  if ((s = socket(AF_INET, SOCK_PACKET, htons(ETH_P_ALL))) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (strcmp(ifname, "") != 0) {
    struct sockaddr sa;

    memset(&sa, 0, sizeof sa);
    sa.sa_family = AF_INET;
    snprintf(sa.sa_data, CMAX, "%s", ifname);
    if (bind(s, &sa, sizeof sa) < 0) {
      perror("bind");
      exit(EXIT_FAILURE);
    }
  }
#else
  if (strcmp(ifname, "") == 0)
    strcpy(ifname, DEF_IF);

  if ((s = open_bpf(ifname, &bufsize)) < 0)
    exit(EXIT_FAILURE);
  bpf_len = 0;
#endif

  while (1) {
    struct ether_header *eth;  /* Ethernet�إå���¤��              */
    struct ether_arp *arp;     /* ARP�ѥ��åȹ�¤��                 */
    struct ip *ip;             /* IP�إå���¤��                    */
    struct icmp *icmp;         /* ICMP�ѥ��åȹ�¤��                */
    struct tcphdr *tcp;        /* TCP�إå���¤��                   */
    struct udphdr *udp;        /* UDP�إå���¤��                   */
    char buff[MAXSIZE];        /* �ǡ��������Хåե�                */
    void *p;                   /* �إå�����Ƭ��ɽ������ѥݥ���  */
    void *p0;                  /* �ѥ��åȤ���Ƭ��ɽ���ݥ���      */
    int len;                   /* ���������ǡ�����Ĺ��              */
    int disp;                  /* ���̤˽��Ϥ������ɤ����Υե饰    */
    struct timeval tv;         /* �ѥ��åȤ����פ�������          */
    struct tm tm;              /* localtime�Ǥλ���ɽ��             */

#ifndef __linux
    /*  BPF��������� */
    if (bpf_len <= 0) {
      /* ʣ���Υѥ��åȤ�����Ф� */
      if ((bpf_len = read(s, buff, bufsize)) < 0) {
        perror("read");
        exit(EXIT_FAILURE);
      }
      bp = (struct bpf_hdr *) buff;
    } else {
      /* BPF�μ��Υѥ��åȤإݥ��󥿤��ư */
      bp = (struct bpf_hdr *) ((char *) bp + bp->bh_hdrlen + bp->bh_caplen);
      bp = (struct bpf_hdr *) BPF_WORDALIGN((int) bp);
    }
    /* �ѥ��åȥ���פλ���򥻥å� */
    memcpy(&tv, &(bp->bh_tstamp), sizeof tv);
    localtime_r((time_t *) &tv.tv_sec, &tm);
    /* Ethernet�إå�����Ƭ�˥ݥ��󥿤򥻥å� */
    p = p0 = (char *) bp + bp->bh_hdrlen;
    len = bp->bh_caplen;
#ifdef DEBUG
    /* BPF�إå���¤���ͤ�ɽ�� */
    printf("bpf_len=%d,",  bpf_len);
    printf("hdrlen=%d,",   bp->bh_hdrlen);
    printf("caplen=%d,",   bp->bh_caplen);
    printf("datalen=%d\n", bp->bh_datalen);
#endif
    /* ����while�롼�פΤ���ν��� */
    bpf_len -= BPF_WORDALIGN(bp->bh_hdrlen + bp->bh_caplen);
#else
    /* Linux SOCK_PACKET��������� */
    if ((len = read(s, buff, MAXSIZE)) < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    /* �ѥ��åȥ���פλ���򥻥å� */
    gettimeofday(&tv, (struct timezone *) 0);
    localtime_r((time_t *) &tv.tv_sec, &tm);
    /* Ethernet�إå�����Ƭ�˥ݥ��󥿤򥻥å� */
    p = p0 = buff;
#endif

    /*
     * �ѥ��åȲ��ϥ롼����
     */
    disp = OFF;  /* ���̤˽��Ϥ��뤫�ɤ����ե饰 */

    /* Ethernet�إå���¤�Τ����� */
    eth = (struct ether_header *) p;
    p += sizeof (struct ether_header);

    if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
      arp = (struct ether_arp *) p;
      if (opt[ARP] == ON && FILTER_ARP(arp))
        disp = ON;
    } else if (ntohs(eth->ether_type) == ETHERTYPE_IP) {
      ip = (struct ip *) p;
      p += ((int) (ip->ip_hl) << 2);

      if (!FILTER_IP(ip))
        continue;
      if (opt[IP] == ON && opt[TCP] != ON && opt[UDP] != ON && opt[ICMP] != ON)
        disp = ON;

      switch (ip->ip_p) {
        case IPPROTO_TCP:
          tcp = (struct tcphdr *) p;
          p += ((int) (tcp->th_off) << 2);
          if (!FILTER_TCP(tcp))
            continue;
          if (opt[TCP] == ON)
            disp = ON;
          break;
        case IPPROTO_UDP:
          udp = (struct udphdr *) p;
          p += sizeof (struct udphdr);
          if (!FILTER_UDP(udp))
            continue;
          if (opt[UDP] == ON)
            disp = ON;
          break;
        case IPPROTO_ICMP:
          icmp = (struct icmp *) p;
          p = icmp->icmp_data;
          if (opt[ICMP] == ON)
            disp = ON;
          break;
        default:
          if (opt[ALL] == ON)
            disp = ON;
          break;
      }
    } else if (opt[ETHER] == ON && opt[ALL] == ON)
      disp = ON;

    /*
     * �ѥ��å�ɽ���롼����
     */
    if (disp == ON || opt[ALL] == ON) {
      if (opt[TIME] == ON)
        printf("Time: %02d:%02d:%02d.%06d\n", tm.tm_hour, tm.tm_min, tm.tm_sec,
               (int) tv.tv_usec);
      if (opt[ETHER] == ON)
        print_ethernet(eth);
      if (ntohs(eth->ether_type) == ETHERTYPE_ARP) {
        if (opt[ARP] == ON)
          print_arp(arp);
      } else if (ntohs(eth->ether_type) == ETHERTYPE_IP) {
        if (opt[IP] == ON)
          print_ip(ip);
        if (ip->ip_p == IPPROTO_TCP && opt[TCP] == ON)
          print_tcp(tcp);
        else if (ip->ip_p == IPPROTO_UDP && opt[UDP] == ON)
          print_udp(udp);
        else if (ip->ip_p == IPPROTO_ICMP && opt[ICMP] == ON)
          print_icmp(icmp);
        else if (opt[ALL] == ON)
          printf("Protocol: unknown\n");
      } else if (opt[ALL] == ON)
        printf("Protocol: unknown\n");
      if (opt[DUMP] == ON)
        dump_packet(p0, len);
      printf("\n");
    }
  }
  return 0;
}

/*
 * char *mac_ntoa(u_char *d);
 * ��ǽ
 *    ����˳�Ǽ����Ƥ���MAC���ɥ쥹��ʸ������Ѵ�
 *    static�ѿ������Ѥ��뤿�ᡢ��ꥨ��ȥ��ȴؿ�
 * ������
 *    u_char *d;  MAC���ɥ쥹����Ǽ����Ƥ����ΰ����Ƭ���ɥ쥹
 * �����
 *    ʸ������Ѵ����줿MAC���ɥ쥹
 */
char *mac_ntoa(u_char *d)
{
#define MAX_MACSTR 50
  static char str[MAX_MACSTR];  /* ʸ������Ѵ�����MAC���ɥ쥹����¸ */

  snprintf(str, MAX_MACSTR, "%02x:%02x:%02x:%02x:%02x:%02x",
           d[0], d[1], d[2], d[3], d[4], d[5]);

  return str;
}

/*
 * void print_ethernet(struct ether_header *eth);
 * ��ǽ
 *     Ethernet�إå���ɽ��
 * ������
 *     struct ether_header *eth;  Ethernet�إå���¤�ΤؤΥݥ���
 * �����
 *     �ʤ�
 */
void print_ethernet(struct ether_header *eth)
{
  int type = ntohs(eth->ether_type);  /* Ethernet������ */

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
    printf("| Ethernet Type:    0x%04x|\n", type);
  printf("+-------------------------+\n");
}

/*
 * void print_arp(struct ether_arp *arp);
 * ��ǽ
 *     ARP�ѥ��åȤ�ɽ��
 * ������
 *     struct ether_arp *arp;  ARP�ѥ��åȹ�¤�ΤؤΥݥ���
 * �����
 *     �ʤ�
 */
void print_arp(struct ether_arp *arp)
{
  static char *arp_op_name[] = {
    "Undefine",
    "(ARP Request)",
    "(ARP Reply)",
    "(RARP Request)",
    "(RARP Reply)"
  };   /* ���ڥ졼�����μ����ɽ��ʸ���� */
#define ARP_OP_MAX (sizeof arp_op_name / sizeof arp_op_name[0])
  int op = ntohs(arp->ea_hdr.ar_op);  /* ARP���ڥ졼����� */

  if (op < 0 || ARP_OP_MAX < op)
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
 * void print_ip(struct ip *ip);
 * ��ǽ
 *     IP�إå���ɽ��
 * ������
 *     struct ip *ip;  IP�إå���¤�ΤؤΥݥ���
 * �����
 *     �ʤ�
 */
void print_ip(struct ip *ip)
{
  printf("Protocol: IP\n");
  printf("+-----+------+------------+-------------------------+\n");
  printf("| IV:%1u| HL:%2u| T: %8s| Total Length: %10u|\n",
         ip->ip_v, ip->ip_hl, ip_ttoa(ip->ip_tos), ntohs(ip->ip_len));
  printf("+-----+------+------------+-------+-----------------+\n");
  printf("| Identifier:        %5u| FF:%3s| FO:        %5u|\n",
         ntohs(ip->ip_id), ip_ftoa(ntohs(ip->ip_off)),
         ntohs(ip->ip_off) &IP_OFFMASK);
  printf("+------------+------------+-------+-----------------+\n");
  printf("| TTL:    %3u| Pro:    %3u| Header Checksum:   %5u|\n",
         ip->ip_ttl, ip->ip_p, ntohs(ip->ip_sum));
  printf("+------------+------------+-------------------------+\n");
  printf("| Source IP Address:                 %15s|\n",
         inet_ntoa(*(struct in_addr *) &(ip->ip_src)));
  printf("+---------------------------------------------------+\n");
  printf("| Destination IP Address:            %15s|\n",
         inet_ntoa(*(struct in_addr *) &(ip->ip_dst)));
  printf("+---------------------------------------------------+\n");
}

/*
 * char *ip_ftoa(int flag);
 * ��ǽ
 *     IP�إå��Υե饰���ȥӥåȤ�ʸ������Ѵ�
 *     static�ѿ�����Ѥ��Ƥ��뤿�ᡢ��ꥨ��ȥ��ȴؿ�
 * ������
 *     int flag;  �ե饰���ȥե�����ɤ���
 * �����
 *     char *     �Ѵ����줿ʸ����
 */
char *ip_ftoa(int flag)
{
  static int  f[] = {'R', 'D', 'M'};  /* �ե饰���ȥե饰��ɽ��ʸ�� */
#define IP_FLG_MAX (sizeof f / sizeof f[0])
  static char str[IP_FLG_MAX + 1];    /* ����ͤ��Ǽ����Хåե�     */
  unsigned int mask = 0x8000;         /* �ޥ���                       */
  int i;                              /* �롼����                     */

  for (i = 0; i < IP_FLG_MAX; i++) {
    if (((flag << i) & mask) != 0)
      str[i] = f[i];
    else
      str[i] = '0';
  }
  str[i] = '\0';

  return str;
}

/*
 * char *ip_ttoa(int flag);
 * ��ǽ
 *     IP�إå���TOS�ե�����ɤ�ʸ������Ѵ�
 *     static�ѿ�����Ѥ��Ƥ��뤿�ᡢ��ꥨ��ȥ��ȴؿ�
 * ������
 *     int flag;  TOS�ե�����ɤ���
 * �����
 *     char *     �Ѵ����줿ʸ����
 */
char *ip_ttoa(int flag)
{
  static int  f[] = {'1', '1', '1', 'D', 'T', 'R', 'C', 'X'}; 
                                 /* TOS�ե�����ɤ�ɽ��ʸ��       */
#define TOS_MAX (sizeof f / sizeof f[0])
  static char str[TOS_MAX + 1];  /* ����ͤ��Ǽ����Хåե�      */
  unsigned int mask = 0x80;      /* TOS�ե�����ɤ���Ф��ޥ��� */
  int i;                         /* �롼���ѿ�                    */

  for (i = 0; i < TOS_MAX; i++) {
    if (((flag << i) & mask) != 0)
      str[i] = f[i];
    else
      str[i] = '0';
  }
  str[i] = '\0';

  return str;
}

/*
 * void print_icmp(struct icmp *icmp);
 * ��ǽ
 *     ICMP�إå����ǡ�����ɽ��
 * ������
 *     struct icmp *icmp;  ICMP�إå���¤��
 * �����
 *     �ʤ�
 */
void print_icmp(struct icmp *icmp)
{
  static char *type_name[] = {
    "Echo Reply",               /* Type  0 */
    "Undefine",                 /* Type  1 */
    "Undefine",                 /* Type  2 */
    "Destination Unreachable",  /* Type  3 */
    "Source Quench",            /* Type  4 */
    "Redirect (change route)",  /* Type  5 */
    "Undefine",                 /* Type  6 */
    "Undefine",                 /* Type  7 */
    "Echo Request",             /* Type  8 */
    "Undefine",                 /* Type  9 */
    "Undefine",                 /* Type 10 */
    "Time Exceeded",            /* Type 11 */
    "Parameter Problem",        /* Type 12 */
    "Timestamp Request",        /* Type 13 */
    "Timestamp Reply",          /* Type 14 */
    "Information Request",      /* Type 15 */
    "Information Reply",        /* Type 16 */
    "Address Mask Request",     /* Type 17 */
    "Address Mask Reply",       /* Type 18 */
    "Unknown"                   /* Type 19 */
  };                            /* ICMP�Υ����פ�ɽ��ʸ���� */
#define ICMP_TYPE_MAX (sizeof type_name / sizeof type_name[0])
  int type = icmp->icmp_type;  /* ICMP������ */

  if (type < 0 || ICMP_TYPE_MAX <= type)
    type = ICMP_TYPE_MAX - 1;

  printf("Protocol: ICMP (%s)\n", type_name[type]);

  printf("+------------+------------+-------------------------+\n");
  printf("| Type:   %3u| Code:   %3u| Checksum:          %5u|\n",
         icmp->icmp_type, icmp->icmp_code, ntohs(icmp->icmp_cksum));
  printf("+------------+------------+-------------------------+\n");

  if (icmp->icmp_type == 0 || icmp->icmp_type == 8) {
      printf("| Identification:    %5u| Sequence Number:   %5u|\n",
             ntohs(icmp->icmp_id), ntohs(icmp->icmp_seq));
      printf("+-------------------------+-------------------------+\n");
  } else if (icmp->icmp_type == 3) {
    if (icmp->icmp_code == 4) {
      printf("| void:          %5u| Next MTU:          %5u|\n",
             ntohs(icmp->icmp_pmvoid), ntohs(icmp->icmp_nextmtu));
      printf("+-------------------------+-------------------------+\n");
    } else {
      printf("| Unused:                                 %10lu|\n",
             (unsigned long) ntohl(icmp->icmp_void));
      printf("+-------------------------+-------------------------+\n");
    }
  } else if (icmp->icmp_type == 5) {
    printf("| Router IP Address:                 %15s|\n",
           inet_ntoa(*(struct in_addr *) &(icmp->icmp_gwaddr)));
    printf("+---------------------------------------------------+\n");
  } else if (icmp->icmp_type == 11) {
    printf("| Unused:                                 %10lu|\n",
           (unsigned long) ntohl(icmp->icmp_void));
    printf("+---------------------------------------------------+\n");
  }

  /* ICMP�θ��ˡ�IP�إå��ȥȥ�󥹥ݡ��ȥإå���³�����ν��� */
  if (icmp->icmp_type == 3 || icmp->icmp_type == 5 || icmp->icmp_type == 11) {
    struct ip *ip = (struct ip *) icmp->icmp_data;    /* IP�إå�             */
    char *p = (char *) ip + ((int) (ip->ip_hl) << 2); /* �ȥ�󥹥ݡ��ȥإå� */

    print_ip(ip);
    switch (ip->ip_p) {
      case IPPROTO_TCP:
        print_tcp_mini((struct tcphdr *) p);
        break;
      case IPPROTO_UDP:
        print_udp((struct udphdr *) p);
        break;
    }
  }
}

/*
 * void print_tcp_mini(struct tcphdr *tcp);
 * ��ǽ
 *     TCP�إå�����Ƭ��64�ӥåȤ�ɽ��(ICMP�������������ʬ)
 * ������
 *     struct tcphdr *tcp;  TCP�إå���¤��
 * �����
 *     �ʤ�
 */
void print_tcp_mini(struct tcphdr *tcp)
{
  printf("Protocol: TCP\n");
  printf("+-------------------------+-------------------------+\n");
  printf("| Source Port:       %5u| Destination Port:  %5u|\n",
         ntohs(tcp->th_sport), ntohs(tcp->th_dport));
  printf("+-------------------------+-------------------------+\n");
  printf("| Sequence Number:                        %10lu|\n",
         (unsigned long) ntohl(tcp->th_seq));
  printf("+---------------------------------------------------+\n");
}

/*
 * void print_tcp(struct tcphdr *tcp);
 * ��ǽ
 *     TCP�إå���ɽ��
 * ������
 *     struct tcphdr *tcp;  TCP�إå���¤��
 * �����
 *     �ʤ�
 */
void print_tcp(struct tcphdr *tcp)
{
  printf("Protocol: TCP\n");
  printf("+-------------------------+-------------------------+\n");
  printf("| Source Port:       %5u| Destination Port:  %5u|\n",
         ntohs(tcp->th_sport), ntohs(tcp->th_dport));
  printf("+-------------------------+-------------------------+\n");
  printf("| Sequence Number:                        %10lu|\n",
         (unsigned long) ntohl(tcp->th_seq));
  printf("+---------------------------------------------------+\n");
  printf("| Acknowledgement Number:                 %10lu|\n",
         (unsigned long) ntohl(tcp->th_ack));
  printf("+------+---------+--------+-------------------------+\n");
  printf("| DO:%2u| Reserved|F:%6s| Window Size:       %5u|\n",
         tcp->th_off, tcp_ftoa(tcp->th_flags), ntohs(tcp->th_win));
  printf("+------+---------+--------+-------------------------+\n");
  printf("| Checksum:          %5u| Urgent Pointer:    %5u|\n",
         ntohs(tcp->th_sum), ntohs(tcp->th_urp));
  printf("+-------------------------+-------------------------+\n");
}

/*
 * char *tcp_ftoa(int flag);
 * ��ǽ
 *     TCP�إå��Υ���ȥ���ե饰��ʸ������Ѵ�
 * ������
 *     int flag;      TCP�Υ���ȥ���ե饰
 * �����
 *     char *         �Ѵ����줿ʸ����
 */
char *tcp_ftoa(int flag)
{
  static int  f[] = {'U', 'A', 'P', 'R', 'S', 'F'}; /* TCP�ե饰��ɽ��ʸ��  */
#define TCP_FLG_MAX (sizeof f / sizeof f[0])
  static char str[TCP_FLG_MAX + 1];            /* ����ͤ��Ǽ����Хåե�  */
  unsigned int mask = 1 << (TCP_FLG_MAX - 1);  /* �ե饰����Ф��ޥ���    */
  int i;                                       /* �롼���ѿ�                */

  for (i = 0; i < TCP_FLG_MAX; i++) {
    if (((flag << i) & mask) != 0)
      str[i] = f[i];
    else
      str[i] = '0';
  }
  str[i] = '\0';

  return str;
}

/*
 * void print_udp(struct udphdr *udp);
 * ��ǽ
 *     UDP�إå���ɽ��
 * ������
 *     struct udphdr *udp;  UDP�إå���¤�ΤؤΥݥ���
 * �����
 *     �ʤ�
 */
void print_udp(struct udphdr *udp)
{
  printf("Protocol: UDP\n");
  printf("+-------------------------+-------------------------+\n");
  printf("| Source Port:       %5u| Destination Port:  %5u|\n",
         ntohs(udp->uh_sport), ntohs(udp->uh_dport));
  printf("+-------------------------+-------------------------+\n");
  printf("| Length:            %5u| Checksum:          %5u|\n",
         ntohs(udp->uh_ulen), ntohs(udp->uh_sum));
  printf("+-------------------------+-------------------------+\n");
}

/*
 * void dump_packet(unsigned char *buff, int len);
 * ��ǽ
 *     Ethernet�ե졼����Ƭ����16�ʿ������(��������ʸ��ɽ��)
 * ������
 *     unsigned char *buff;  ����פ���ǡ�������Ƭ���ɥ쥹
 *     int len;              ����פ���Х��ȿ�
 * �����
 *     �ʤ�
 */
void dump_packet(unsigned char *buff, int len)
{
  int i, j;  /* �롼���ѿ� */

  printf("Frame Dump:\n");
  for (i = 0; i < len; i += 16) {
    /* 16�ʿ������ */
    for (j = i; j < i + 16 && j < len; j++) {
      printf("%02x", buff[j]);
      if (j % 2)
        printf(" ");
    }

    /* �Ǹ�ιԤ�ü�������� */
    if (j == len && len % 16 != 0)
      for (j = 0; j < 40 - (len % 16) * 2.5; j++)
        printf(" ");
    printf(": ");

    /* ��������ʸ��ɽ�� */
    for (j = i; j < i + 16 && j < len; j++) {
      if ((buff[j] >= 0x20) && (buff[j] <= 0x7e))
        putchar(buff[j]);
      else
        printf(".");
    }

    printf("\n");
  }
  fflush(stdout);
}

#ifndef __linux
/*
 * int open_bpf(char *ifname, int *bufsize);
 * ��ǽ
 *     BPF�򥪡��ץ󤹤�
 * ������
 *     char *ifname;  ���󥿥ե�����̾
 *     int *bufsize;  BPF�����ΥХåե�������
 * �����
 *     int            �ե�����ǥ�������ץ�
 */
int open_bpf(char *ifname, int *bufsize)
{
  char buf[CMAX];    /* ʸ�����Ǽ��              */
  int bpfd;          /* �ե�����ǥ�������ץ�    */
  struct ifreq ifr;  /* ���󥿥ե�����°����¤��  */
  int i;             /* �롼���ѿ�                */

  /* BPF�ǥХ����ե�����Υ����ץ� */
  for (i = 0; i < 4; i++) { 
    snprintf(buf, CMAX, "/dev/bpf%d", i);
    if ((bpfd = open(buf, O_RDWR, 0)) > 0)
      break;
  }

  if (i >= 5) {
    fprintf(stderr, "cannot open BPF\n");
    return -1;
  }

  /* BPF�����ΥХåե������������� */
  *bufsize = MAXSIZE;
  if (ioctl(bpfd, BIOCSBLEN, bufsize) < 0) {
    snprintf(buf, CMAX, "ioctl(BIOCSBLEN, %d)", *bufsize);
    perror(buf);
    return -1;
  }

  /* ���󥿥ե�����̾������ */
  snprintf(ifr.ifr_name, CMAX, "%s", ifname);
  if (ioctl(bpfd, BIOCSETIF, &ifr) < 0) {
    snprintf(buf, CMAX, "ioctl(BIOCSETIF, '%s')", ifname);
    perror(buf);
    return -1;
  }
  fprintf(stderr, "BPF read from '%s' (%s)\n", ifr.ifr_name, buf);

  /* promiscuous�⡼�� */
  if (ioctl(bpfd, BIOCPROMISC, NULL) < 0) {
    perror("ioctl(BIOCPROMISC)");
    return -1;
  }

  /* ¨���⡼�� */
  i = 1;
  if (ioctl(bpfd, BIOCIMMEDIATE, &i) < 0) {
    perror("ioctl(BIOCIMMEDIATE)");
    return -1;
  }

  return bpfd;
}
#endif

void help(char *cmd)
{
  fprintf(stderr, "usage: %s [-aedht] [-i ifname] [-p protocols] [-f filters]\n", cmd);
  fprintf(stderr, "protocols: arp ip icmp tcp udp other\n");
  fprintf(stderr, "filters: ip <ip addr> port <port number>\n");
#ifdef __linux
  fprintf(stderr, "default: %s -p arp ip icmp tcp udp\n", cmd);
#else
  fprintf(stderr, "default: %s -i %s -p arp ip icmp tcp udp\n", cmd, DEF_IF);
#endif
}
