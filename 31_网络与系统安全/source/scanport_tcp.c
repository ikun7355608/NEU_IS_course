/**********************************************************************
 *  TCP�ݡ��ȥ������ץ���� (scanport_tcp.c)
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
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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

enum {CMD_NAME, DST_IP, START_PORT, LAST_PORT};
enum {CONNECT, NOCONNECT};

int tcpportscan(u_int32_t dst_ip, int dst_port);

int main(int argc, char *argv[]) 
{
  u_int32_t dst_ip;  /* ����IP���ɥ쥹          */
  int dst_port;      /* �����ݡ����ֹ�          */
  int start_port;    /* ������󳫻ϥݡ����ֹ�  */
  int end_port;      /* �������λ�ݡ����ֹ�  */

  /* ���ޥ�ɥ饤��������Υ����å� */
  if (argc != 4) {
    fprintf(stderr, "usage: %s dst_ip start_port last_port\n", argv[CMD_NAME]);
    exit(EXIT_FAILURE);
  }

  /* ���ޥ�ɥ饤������������ͤ򥻥åȤ��� */
  dst_ip     = inet_addr(argv[DST_IP]);
  start_port = atoi(argv[START_PORT]);
  end_port   = atoi(argv[LAST_PORT]);

  CHKADDRESS(dst_ip);

  /* �ᥤ��롼���� */
  for (dst_port = start_port; dst_port <= end_port; dst_port++) {
    printf("Scan Port %d\r", dst_port);
    fflush(stdout);

    if (tcpportscan(dst_ip, dst_port) == CONNECT) {
      struct servent *se;  /* �����ӥ����� */

      se = getservbyport(htons(dst_port), "tcp");
      printf("%5d %-20s\n", dst_port, (se == NULL) ? "unknown" : se->s_name);
    }
  }

  return 0;
}

/*
 * int tcpportscan(u_int32_t dst_ip, int dst_port)
 * ��ǽ
 *     ���ꤷ��IP���ɥ쥹���ݡ����ֹ��TCP���ͥ�����󤬳�Ω�Ǥ��뤫Ĵ�٤롣
 * ������ 
 *     u_int32_t dst_ip;  ������IP���ɥ쥹 (�ͥåȥ���Х��ȥ�������)
 *     int dst_port;      ������ݡ����ֹ�
 * �����
 *     int                CONNECT  ... ���ͥ�����󤬳�Ω�Ǥ���
 *                        NOCONNECT .. ���ͥ�����󤬳�Ω�Ǥ��ʤ��ä�
 */
int tcpportscan(u_int32_t dst_ip, int dst_port)
{
  struct sockaddr_in dest;  /* �����åȥ��ɥ쥹       */
  int s;                    /* �����åȥǥ�������ץ� */
  int ret;                  /* ���δؿ��������       */

  /* �������ɥ쥹������ */
  memset(&dest, 0, sizeof dest);
  dest.sin_family      = AF_INET;
  dest.sin_port        = htons(dst_port);
  dest.sin_addr.s_addr = dst_ip;

  /* TCP�����åȤΥ����ץ� */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* TCP���ͥ�������Ω�׵� */
  if (connect(s, (struct sockaddr *) &dest, sizeof dest) < 0)
    ret = NOCONNECT;
  else
    ret = CONNECT;
  close(s);

  return ret;
}
