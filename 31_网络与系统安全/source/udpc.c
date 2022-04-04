/**********************************************************************
 *  UDP����ץ륯�饤����� (udpc.c)
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>

#define MSGSIZE      1024
#define BUFSIZE      (MSGSIZE + 1)
#define DEFAULT_PORT 5320

enum {CMD_NAME, DST_IP, DST_PORT};

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* �����ФΥ��ɥ쥹        */
  unsigned long dst_ip;       /* �����Ф�IP���ɥ쥹      */
  int port;                   /* �ݡ����ֹ�              */
  int s;                      /* �����åȥǥ�������ץ�  */
  unsigned int zero;          /* ����                    */

  /* �������Υ����å� */
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s  hostname  [port]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* �����Ф�IP���ɥ쥹��Ĵ�٤� */
  if ((dst_ip = inet_addr(argv[DST_IP])) == INADDR_NONE) {
    struct hostent *he;  /* �ۥ��Ⱦ��� */

    if ((he = gethostbyname(argv[DST_IP])) == NULL) {
      fprintf(stderr, "gethostbyname error\n");
      exit(EXIT_FAILURE);
    }
    memcpy((char *) &dst_ip, (char *) he->h_addr, he->h_length);
  }

  /* �����ФΥݡ����ֹ��Ĵ�٤� */
  if (argc == 3) {
    if ((port = atoi(argv[DST_PORT])) == 0) {
      struct servent *se;  /* �����ӥ����� */

      if ((se = getservbyname(argv[DST_PORT], "udp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* UDP�ǥ����åȤ򳫤� */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* �����ФΥ��ɥ쥹�����ꤹ�� */
  memset(&server, 0, sizeof server);
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = dst_ip;
  server.sin_port        = htons(port);

  /*
   * ���饤����Ƚ����ᥤ��롼����
   */
  zero = 0;
  printf("UDP>");
  fflush(stdout);

  while (1) {
    char buf[BUFSIZE];  /* �����Хåե�                   */
    char cmd[BUFSIZE];  /* �����Хåե�                   */
    int n;              /* ���ϥǡ����ΥХ��ȿ�           */
    struct timeval tv;  /* select�Υ����ॢ���Ȼ���       */
    fd_set readfd;      /* select�Ǹ��Ф���ǥ�������ץ� */

    /* select�Υ����ॢ���Ȥ����� */
    tv.tv_sec  = 600;
    tv.tv_usec = 0;

    /* select�ˤ�륤�٥���Ԥ� */
    FD_ZERO(&readfd);
    FD_SET(0, &readfd);  /* ɸ������ */
    FD_SET(s, &readfd);  /* ������   */
    if ((select(s + 1, &readfd, NULL, NULL, &tv)) <= 0) {
      fprintf(stderr, "\nTimeout\n");
      break;
    }

    /* ɸ�����Ϥ�������ϥ��ޥ�ɤν��� */
    if (FD_ISSET(0, &readfd)) {
      if ((n = read(0, buf, BUFSIZE - 1)) <= 0)
        break;
      buf[n] = '\0';
      sscanf(buf, "%s", cmd);
      if (strcmp(cmd, "quit") == 0)
        break;
      if (sendto(s, buf, n, 0, (struct sockaddr *) &server, sizeof server) < 0)
        break;
    }

    /* �����Ф���α�����å������ν��� */
    if (FD_ISSET(s, &readfd)) {
      if ((n = recvfrom(s, buf, MSGSIZE, 0, (struct sockaddr *) 0, &zero)) < 0)
        break;
      buf[n] = '\0';
      printf("%s", buf);
      if (n < MSGSIZE)
        printf("UDP>");
      fflush(stdout);
    }
  }
  close(s);

  return 0;
}
