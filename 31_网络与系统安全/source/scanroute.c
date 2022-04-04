/**********************************************************************
 *  �롼�ȥ������ץ���� (scanroute.c)
 *        Ver 2.0 2004ǯ 7�� 10��
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
  struct packet_udp sendpacket;  /* ��������UDP/IP�ѥ��å�          */
  struct sockaddr_in send_sa;    /* �����Υ��ɥ쥹                  */
  int send_sd;                   /* �����ѥ����åȥǥ�������ץ�    */
  int recv_sd;                   /* �����ѥ����åȥǥ�������ץ�    */
  int len;                       /* ��������IP�ѥ��å�Ĺ            */
  int ttl;                       /* TTL����                         */
  u_char buff[BUFSIZE];          /* �����Хåե�                    */
  struct icmp *icmp;             /* ICMP��¤��                      */
  struct ip *ip;                 /* IP��¤��                        */
  struct in_addr ipaddr;         /* IP���ɥ쥹��¤��                */
  int on = 1;                    /* ON                              */
  int dns_flg = ON;              /* �ۥ��ȤΥɥᥤ��̾�򸡺����뤫  */

  /* ������-n���ץ����Υ����å� */
  if (argc == 3 && strcmp(argv[1], "-n") == 0) {
    dns_flg = OFF;
    argv[1] = argv[2];
    argv[2] = NULL;
    argc = 2;
  }

  /* �������Υ����å� */
  if (argc != 2) {
    fprintf(stderr, "usage: %s [-n] dst_ip\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  memset(&send_sa, 0, sizeof send_sa);
  send_sa.sin_family = AF_INET;

  /* �ɥᥤ��̾����IP���ɥ쥹�ؤ��Ѵ� */
  if ((send_sa.sin_addr.s_addr = inet_addr(argv[DST_IP])) == INADDR_NONE) {
    struct hostent *he;  /* �ۥ��Ⱦ��� */

    if ((he = gethostbyname(argv[DST_IP])) == NULL) {
      fprintf(stderr, "unknown host %s\n", argv[DST_IP]);
      exit(EXIT_FAILURE);
    }
    send_sa.sin_family = he->h_addrtype;
    memcpy((char *) &(send_sa.sin_addr), he->h_addr, sizeof he->h_length);
  }

  /* UDP/IP������raw�����åȤΥ����ץ� */
  if ((send_sd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }
  /* IP�إå������Ǻ������� */
  if (setsockopt(send_sd, IPPROTO_IP, IP_HDRINCL, &on, sizeof on) < 0) {
    perror("setsockopt(IPPROTO_IP, IP_HDRINCL)");
    exit(EXIT_FAILURE);
  }

  /* ICMP������raw�����åȤΥ����ץ� */
  if ((recv_sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }

  /* UDP/IP�ѥ��åȤκ��� */
  len = sizeof (struct packet_udp);
  memset(&sendpacket, 0, sizeof sendpacket);
  make_udp_header(&(sendpacket.udp));
  make_ip_header(&(sendpacket.ip), 0, send_sa.sin_addr.s_addr, len);

  /*
   * �������롼�ȥᥤ��롼����
   */
  printf("scanroute %s\n", inet_ntoa(send_sa.sin_addr));
  for (ttl = 1; ttl <= 64; ttl++) {
    struct timeval tvm0;  /* �ѥ��å���������     */
    struct timeval tvm1;  /* �ѥ��åȼ�������     */
    struct timeval tv;    /* select�Υ����ॢ���� */
    int i;                /* �롼���ѿ�           */

    printf("%2d: ", ttl);
    fflush(stdout);

    sendpacket.ip.ip_ttl = ttl;

    for (i = 0; i < 3; i++) {
      /* UDP�ѥ��åȤ����� */
      if (sendto(send_sd, (char *) &sendpacket, len, 0,
                 (struct sockaddr *) &send_sa, sizeof send_sa) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }
      /* ��������ε�Ͽ */
      gettimeofday(&tvm0, (struct timezone *) 0);

      /* select�Υ����ॢ���Ȥ����� */
      tv.tv_sec  = 3;
      tv.tv_usec = 0;

      /* �ѥ��åȼ����롼�� */
      while (1) {
        fd_set readfd;  /* select�Ǹ�������ǥ�������ץ� */
        int n;          /* select�������                 */
        int ihlen;      /* �إå�Ĺ                       */

        /* select�ǥѥ��åȤ��������ޤ��Ԥ� */
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

        /* ICMP�ѥ��åȤμ��� */
        if (recvfrom(recv_sd, buff, BUFSIZE, 0, NULL, NULL) < 0) {
          perror("recvfrom");
          exit(EXIT_FAILURE);
        }
        /* �ѥ��åȼ�������ε�Ͽ */
        gettimeofday(&tvm1, (struct timezone *) 0);

        /* �ط�����ѥ��åȤ��ɤ����򸡺� */
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
      struct hostent *host;          /* �ۥ��Ⱦ���              */
      char hostip[INET_ADDRSTRLEN];  /* IP���ɥ쥹��ɽ��ʸ����  */

      /* IP���ɥ쥹�ȥɥᥤ��̾��ɽ�� */
      memcpy(&ipaddr, &(ip->ip_src.s_addr), sizeof ipaddr);
      snprintf(hostip, INET_ADDRSTRLEN, "%s", inet_ntoa(*(struct in_addr *)
                                                        &(ip->ip_src.s_addr)));
      if (dns_flg == OFF)
        printf("%-15s", hostip);
      else if ((host = gethostbyaddr((char *) &ipaddr, 4, AF_INET)) == NULL)
        printf("%-15s (%s) ", hostip, hostip);
      else
        printf("%-15s (%s) ", hostip, host->h_name);

      /* �������֤�ɽ�� */
      tvsub(&tvm1, &tvm0);
      printf(": RTT =%8.4f ms", tvm1.tv_sec * 1000.0 + tvm1.tv_usec / 1000.0);

      /* ɸŪ�ۥ��Ȥ����夷�����ɤ����θ��� */
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
 * ��ǽ
 *     UDP�إå��κ���
 * ������
 *     struct udphdr *udp;  ��������UDP�إå�����Ƭ���ɥ쥹
 * �����
 *     �ʤ�
 */
void make_udp_header(struct udphdr *udp)
{
  udp->uh_sport = htons(33434);
  udp->uh_ulen  = htons((u_int16_t) sizeof (struct udphdr));
  udp->uh_dport = htons(33434);  /* traceroute�Υݡ����ֹ� */
  udp->uh_sum   = htons(0);
}

/*
 * void make_ip_header(struct ip *ip, int srcip, int dstip, int iplen);
 * ��ǽ
 *     IP�إå��κ���
 * ������
 *     struct ip *ip;  ��������IP�إå�����Ƭ���ɥ쥹
 *     int srcip;      ������IP���ɥ쥹
 *     int dstip;      ������IP���ɥ쥹
 *     int iplen;      IP�ǡ�����������Ĺ
 * �����
 *     �ʤ�
 */
void make_ip_header(struct ip *ip, int srcip, int dstip, int iplen)
{
  memset(ip, 0, sizeof ip);

  /* IP�إå��κ��� */
  ip->ip_v   = IPVERSION;
  ip->ip_hl  = sizeof (struct ip) >> 2;
  ip->ip_id  = htons(0);
  ip->ip_off = 0;

#ifdef __linux
  /* Linux��raw IP�ξ�� */
  ip->ip_len = htons(iplen);
  ip->ip_off = htons(0);
#else
  /* BSD��raw IP�ξ�� */
  ip->ip_len = iplen;
  ip->ip_off = 0;
#endif
  ip->ip_ttl = 64;
  ip->ip_p   = IPPROTO_UDP;
  ip->ip_src.s_addr = srcip;
  ip->ip_dst.s_addr = dstip;

  /* �����å�����η׻� */
  ip->ip_sum = 0;
}

/*
 * void tvsub(struct timeval *out, struct timeval *in);
 * ��ǽ
 *     strcut timeval�ΰ���������̤�out�˳�Ǽ����롣
 * ������ 
 *     struct timeval *out;  ��������
 *     struct timeval *in;   ������
 * �����
 *     �ʤ�
 */
void tvsub(struct timeval *out, struct timeval *in)
{
  if ((out->tv_usec -= in->tv_usec) < 0) {
    out->tv_sec--;
    out->tv_usec += 1000000;
  }
  out->tv_sec -= in->tv_sec;
}
