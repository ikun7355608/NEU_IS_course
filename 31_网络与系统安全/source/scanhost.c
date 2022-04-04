/**********************************************************************
 *  �ۥ��ȥ������ץ���� (scanhost.c)
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
  struct sockaddr_in send_sa;  /* ������Υ��ɥ쥹                */
  int s;                       /* �����åȥǥ�������ץ�          */
  char send_buff[PACKET_LEN];  /* �����Хåե�                    */
  char recv_buff[BUFSIZE];     /* �����Хåե�                    */
  int start_ip;                /* ������󤹤�IP���ɥ쥹�γ�����  */
  int end_ip;                  /* ������󤹤�IP���ɥ쥹�ν�λ��  */
  int dst_ip;                  /* ������󤹤�IP���ɥ쥹����      */
  int on = 1;                  /* ON                              */

  if (argc != 3) {
    fprintf(stderr, "usage: %s start_ip last_ip\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* ������󤹤�IP���ɥ쥹���ϰϤ����� */
  start_ip = ntohl(inet_addr(argv[START_IP]));
  end_ip   = ntohl(inet_addr(argv[LAST_IP]));

  memset(&send_sa, 0, sizeof send_sa);
  send_sa.sin_family = AF_INET;

  /* ICMP/IP��������raw�����åȤΥ����ץ� */
  if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
    perror("socket(SOCK_RAW, IPPROTO_ICMP)");
    exit(EXIT_FAILURE);
  }
  /* BROADCAST�ѥ��åȤ������Ǥ���褦�ˤ��� */
  if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof on) < 0) {
    perror("setsockopt(SOL_SOCKET, SO_BROADCAST)");
    exit(EXIT_FAILURE);
  }

  /*
   * �������ۥ��ȥᥤ��롼����
   */
  for (dst_ip = start_ip; dst_ip <= end_ip; dst_ip++) {
    int i;  /* �롼���ѿ� */

    send_sa.sin_addr.s_addr = htonl(dst_ip);
    CHKADDRESS(send_sa.sin_addr);

    for (i = 0; i < 3; i++) {
      struct timeval tv;  /* ����ξ��� */

      printf("scan %s (%d)\r", inet_ntoa(send_sa.sin_addr), i + 1);
      fflush(stdout);

      /* ICMP�������׵�ѥ��å�(������������)������������� */
      make_icmp8_packet((struct icmp *) send_buff, PACKET_LEN, i);
      if (sendto(s, (char *) &send_buff, PACKET_LEN, 0,
                 (struct sockaddr *) &send_sa, sizeof send_sa) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }

      /* select�Υ����ॢ���Ȥ����� */
      tv.tv_sec  = 0;
      tv.tv_usec = 200 * 1000;

      while (1) {
        fd_set readfd;  /* select����ե�����ǥ�������ץ� */
        struct ip *ip;  /* ip�إå���¤��   */
        int ihlen;      /* �إå�Ĺ   */

        /* select�ǥѥ��åȤ�������Ԥ� */
        FD_ZERO(&readfd);
        FD_SET(s, &readfd);
        if (select(s + 1, &readfd, NULL, NULL, &tv) <= 0)
          break;

        /* �ѥ��åȼ��� */
        if (recvfrom(s, recv_buff, BUFSIZE, 0, NULL, NULL) < 0) {
          perror("recvfrom");
          exit(EXIT_FAILURE);
        }

        ip = (struct ip *) recv_buff;
        ihlen = ip->ip_hl << 2;
        if (ip->ip_src.s_addr == send_sa.sin_addr.s_addr) {
          struct icmp *icmp;  /* icmp�ѥ��åȹ�¤�� */

          icmp = (struct icmp *) (recv_buff + ihlen);
          if (icmp->icmp_type == ICMP_ECHOREPLY) {
            /* ICMP�������������֤��Ƥ���IP���ɥ쥹��ɽ�� */
            printf("%-15s", inet_ntoa(*(struct in_addr *)
                                      &(ip->ip_src.s_addr)));
            /* ��������(RTT)�η׻���ɽ�� */
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
 * ��ǽ
 *     ICMP�ѥ��åȤκ���
 * ������ 
 *     struct icmp *icmp;  ��������ICMP�إå�����Ƭ���ɥ쥹
 *     int len;            ICMP�������׵�Υѥ��å�Ĺ
 *     int n;              ICMP�������׵�Υ��������ֹ�
 * �����
 *     �ʤ�
 */
void make_icmp8_packet(struct icmp *icmp, int len, int n)
{
  memset(icmp, 0, len);

  /* ��������ǡ������˵�Ͽ���ͥåȥ���Х��ȥ��������򵤤ˤ��ʤ� */
  gettimeofday((struct timeval *) (icmp->icmp_data), (struct timezone *) 0);

  /* ICMP�إå��κ��� */
  icmp->icmp_type = ICMP_ECHO;
  icmp->icmp_code = 0;
  icmp->icmp_id   = 0;  /* ����ID�ˤϥץ���ID�ʤɤμ��̻Ҥ��Ǽ���� */
  icmp->icmp_seq  = n;  /* �ͥåȥ���Х��ȥ��������򵤤ˤ��ʤ� */

  /* �����å�����η׻� */
  icmp->icmp_cksum = 0;
  icmp->icmp_cksum = checksum((u_int16_t *) icmp, len);
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
