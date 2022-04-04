/**********************************************************************
 *  UDP�ݡ��ȥ������ץ���� (scanport_udp.c)
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
  int recv_sd;       /* �����ѥǥ�������ץ�                      */
  int send_sd;       /* �����ѥǥ�������ץ�                      */
  u_int32_t dst_ip;  /* ����IP���ɥ쥹 (�ͥåȥ���Х��ȥ�����) */
  int end_port;      /* ������󳫻ϥݡ����ֹ�                    */
  int start_port;    /* �������λ�ݡ����ֹ�                    */
  int dst_port;      /* ������󤹤�ݡ����ֹ�                    */

  /* ���ޥ�ɥ饤��������ο��Υ����å� */
  if (argc != 4) {
    fprintf(stderr, "usage: %s dst_ip start_port last_port\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* ���ޥ�ɥ饤������������ͤ򥻥åȤ��� */
  dst_ip     = inet_addr(argv[DST_IP]);
  start_port = atoi(argv[START_PORT]);
  end_port   = atoi(argv[LAST_PORT]);

  CHKADDRESS(dst_ip);

  /* ������UDP�����åȤΥ����ץ� */
  if ((send_sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket(SOCK_DGRAM)");
    exit(EXIT_FAILURE);
  }

  /* ������raw�����åȤΥ����ץ� */
  if ((recv_sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }

  /* �������ݡ��ȥᥤ��롼�� */
  for (dst_port = start_port; dst_port <= end_port; dst_port++) {
    printf("Scan Port %d\r", dst_port);
    fflush(stdout);

    if (udpportscan(dst_ip, dst_port, send_sd, recv_sd) == CONNECT) {
      /* select�������ॢ���Ȥ����Ȥ� */
      struct servent *se;  /* �����ӥ����� */

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
 * ��ǽ
 *     ���ꤷ��IP���ɥ쥹���ݡ����ֹ��UDP���̿��Ǥ��뤫Ĵ�٤롣
 * ������ 
 *     u_int32_t dst_ip;  ������IP���ɥ쥹 (�ͥåȥ���Х��ȥ�����)
 *     int dst_port;      ������ݡ����ֹ�
 * �����
 *     int                CONNECT  ... �ݡ��Ȥ������Ƥ���
 *                        NOCONNECT .. �ݡ��Ȥ��ĤޤäƤ���
 */
int udpportscan(u_int32_t dst_ip, int dst_port, int send_sd, int recv_sd)
{
  struct sockaddr_in send_sa;  /* ������Υ��ɥ쥹����      */
  char buff[MAXBUFF];          /* �ѥ��åȼ����ѥХåե�    */
  struct timeval tv;           /* select�Υ����ॢ���Ȼ���  */
  fd_set select_fd;            /* select�ѥӥåȥޥå�      */

  /* �����襢�ɥ쥹������ */
  memset(&send_sa, 0, sizeof send_sa);
  send_sa.sin_family      = AF_INET;
  send_sa.sin_addr.s_addr = dst_ip;
  send_sa.sin_port        = htons(dst_port);

  /* UDP�ѥ��åȤ����� (�إå��Τ��������ǡ�����0�Х���) */
  sendto(send_sd, NULL, 0, 0, (struct sockaddr *) &send_sa, sizeof send_sa);

  /* select�Υ����ॢ���Ȥ����� */
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  while (1) {
    /* select�Ǹ�������ǥ�������ץ������� */
    FD_ZERO(&select_fd);
    FD_SET(recv_sd, &select_fd);

    if (select(recv_sd + 1, &select_fd, NULL, NULL, &tv) > 0) {
      struct icmp *icmp;   /* icmp�إå�                    */
      struct ip *ip;       /* IP�إå���¤��                */
      int ihlen;           /* IP�إå���Ĺ��                */
      struct ip *ip2;      /* ICMP�ǡ������IP�إå���¤��  */
      int ihlen2;          /* ICMP�ǡ������IP�إå���Ĺ��  */
      struct udphdr *udp;  /* �ݡ����ֹ�                    */

      /* ICMP�ѥ��åȤμ��� (56 = 20(IP) + 8(ICMP) + 20(IP) + 8(UDP)) */
      if (recvfrom(recv_sd, buff, MAXBUFF, 0, NULL, NULL) < 56)
        continue;

      ip     = (struct ip *) buff;
      ihlen  = ip->ip_hl << 2;
      icmp   = (struct icmp *) ((char *) ip + ihlen);
      ip2    = (struct ip *) icmp->icmp_data;
      ihlen2 = ip2->ip_hl << 2;
      udp    = (struct udphdr *) ((char *) ip2 + ihlen2);

      /* ���������ѥ��åȤ��ݡ�����ã��ǽ�ʤ�while�롼�פ�ȴ���� */
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
