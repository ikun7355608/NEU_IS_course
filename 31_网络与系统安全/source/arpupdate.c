/**********************************************************************
 *  ARP�ơ��֥빹���ץ���� (arpupdate.c)
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
  int s;               /* �����åȥǥ�������ץ�          */
  u_char mac_addr[6];  /* MAC���ɥ쥹��Ǽ��               */
  int tmp[6];          /* MAC���ɥ쥹��Ǽ��(�������)     */
  int flag;            /* �ե饰 (REPLY��REQUEST��NORMAL) */
  int i;               /* �롼���ѿ�                      */
  u_int32_t dst_ip;    /* �������å�IP���ɥ쥹            */
  char ifname[CMAX];   /* ���󥿥ե�����̾                */
#ifndef __linux
  struct bpf_hdr *bp;  /* BPF�إå���¤��                 */
  int bpf_len;         /* BPF�Ǥμ����ǡ�����Ĺ��         */
  int bufsize;         /* BPF�����ΥХåե�������         */
#else
  struct sockaddr sa;  /* �����åȥ��ɥ쥹��¤��          */
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
    struct ether_header *eth;  /* Ethernet�إå���¤��                      */
    char recv_buff[MAXSIZE];   /* �����Хåե�                              */
    char send_buff[MAXSIZE];   /* �����Хåե�                              */
    char *rp;                  /* �إå�����Ƭ��ɽ������ѥݥ���(������)  */
    char *rp0;                 /* �ѥ��åȤ���Ƭ��ɽ���ݥ���(������)      */
    char *sp;                  /* �إå�����Ƭ��ɽ������ѥݥ���(������)  */
    int len;                   /* �Ƽ��Ĺ��(�������)                      */

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

    /* arpupdate�����������ѥ��åȵڤӤ����ֻ���̵�뤹�� */
    if ( memcmp(eth->ether_dhost, mac_addr, 6) != 0 
      && memcmp(eth->ether_shost, mac_addr, 6) != 0
      && ntohs(eth->ether_type) == ETHERTYPE_ARP) {
      struct ether_arp *arp;  /* ARP�ѥ��åȹ�¤�� */

      arp = (struct ether_arp *) rp;

      if (dst_ip == *(int *) (arp->arp_spa)) {
        static u_char zero[6]; /* Ethernet NULL MAC���ɥ쥹 */
        static u_char one[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
                               /* Ethernet�֥��ɥ��㥹��MAC���ɥ쥹 */

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

        /* �ѥ��åȼ����塢0.5���ԤäƤ���ѥ��åȤ����� */
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
 * ��ǽ
 *     Ethernet�إå��κ���
 * ������ 
 *     struct ether_header *eth;    ��������إå�Ethernet�إå�
 *     unsigend char *ether_dhost;  ����MAC���ɥ쥹
 *     unsigend char *ether_shost;  ����MAC���ɥ쥹
 *     u_int16_t ether_type;        ������
 * �����
 *     �ʤ�
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
 * ��ǽ
 *     ARP�ѥ��åȤκ���
 * ������ 
 *     struct ether_arp *arp;   ��������ARP�إå�
 *     int op;                  ���ڥ졼�����
 *     unsigned char *arp_sha;  ����MAC���ɥ쥹
 *     unsigned *arp_spa;       ����IP���ɥ쥹
 *     unsigned *arp_tha;       �������å�MAC���ɥ쥹
 *     unsigned *arp_tpa;       �������å�IP���ɥ쥹
 * �����
 *     �ʤ�
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
 * ��ǽ
 *    ����˳�Ǽ����Ƥ���MAC���ɥ쥹��ʸ������Ѵ�
 *    static�ѿ������Ѥ��뤿�ᡢ��ꥨ��ȥ��ȴؿ�
 * ������
 *    unsigned char *d;  MAC���ɥ쥹����Ǽ����Ƥ����ΰ����Ƭ���ɥ쥹
 * �����
 *    ʸ������Ѵ����줿MAC���ɥ쥹
 */
char *mac_ntoa(unsigned char *d)
{
#define MACSTRLEN 50
  static char str[MACSTRLEN];  /* ʸ������Ѵ�����MAC���ɥ쥹����¸ */

  snprintf(str, MACSTRLEN, "%02x:%02x:%02x:%02x:%02x:%02x",
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
  int type = ntohs(eth->ether_type); /* Ethernet������ */

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
 * ��ǽ
 *     arpupdate�λ�����ˡ��ɽ��
 * ������
 *     char *cmd;    ���ޥ��̾ (arpupdate)
 * �����
 *     �ʤ�
 */
void help(char *cmd)
{
  fprintf(stderr, "usage: %s ifname dst_ip mac_addr [reply|request]\n", cmd);
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
  char buf[CMAX];    /* ʸ�����Ǽ��  */
  int bpfd;          /* �ե�����ǥ�������ץ�  */
  struct ifreq ifr;  /* ���󥿥ե�����°����¤��  */
  int i;             /* �롼���ѿ�    */

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
