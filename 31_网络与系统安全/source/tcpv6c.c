/**********************************************************************
 *  TCP����ץ륯�饤����ȥץ���� (tcpv6c.c)
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
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE      8192
#define DEFAULT_PORT "5320"

enum {CMD_NAME, DST_IP, DST_PORT};

int main(int argc, char *argv[])
{
  struct addrinfo info;    /* ��³��ξ���                 */
  struct addrinfo *res0;   /* ���ɥ쥹�ꥹ�Ȥ���Ƭ�ݥ��� */
  struct addrinfo *res;    /* ���ɥ쥹�ꥹ��               */
  char ip[NI_MAXHOST];     /* IP���ɥ쥹��ʸ����Ȥ��Ƴ�Ǽ */
  int s;                   /* �����åȥǥ�������ץ�       */
  int n;                   /* ���ϥǡ����ΥХ��ȿ�         */
  int len;                 /* ���ץꥱ�������ǡ�����Ĺ�� */
  char send_buf[BUFSIZE];  /* �����Хåե�                 */
  int error;               /* getaddrinfo�Υ��顼������    */

  /* �������Υ����å� */
  if (argc != 2 && argc != 3) {
    fprintf(stderr, "Usage: %s  hostname  [port]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* IP���ɥ쥹���ݡ����ֹ������(DNS�ؤ��䤤��碌) */
  memset(&info, 0, sizeof info);
  info.ai_family   = AF_UNSPEC;
  info.ai_socktype = SOCK_STREAM;
  error = getaddrinfo(argv[DST_IP], (argc == 3) ? argv[DST_PORT] : DEFAULT_PORT,
                      &info, &res0);
  if (error) {
    fprintf(stderr, "'%s' %s\n", argv[DST_IP], gai_strerror(error));
    exit(EXIT_FAILURE);
  }

  /* ���ͥ������γ�Ω�롼�� */
  for (res = res0; res; res = res->ai_next) {
    /* �����åȤ򳫤� */
    s = -1;
    if ((s = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) >= 0) {
      /* ��³���륵���ФΥ��ɥ쥹�ȥץ�ȥ���μ����ɽ�� */
      if (getnameinfo((struct sockaddr *) res->ai_addr, res->ai_addrlen, ip,
                      sizeof ip, NULL, 0, NI_NUMERICHOST)) {
        fprintf(stderr, "getnameinfo error\n");
        exit(EXIT_FAILURE);
      }
      printf("connecting '%s' ", ip);
      if (res->ai_addr->sa_family == AF_INET6)
        printf("by IPv6. ");
      else if (res->ai_addr->sa_family == AF_INET)
        printf("by IPv4. ");
      else
        printf("by unkown protocol. ");
      fflush(stdout);

      /* ���ͥ�������Ω�׵ᳫ�� */
      if (connect(s, res->ai_addr, res->ai_addrlen) >= 0) {
        printf("connected.\n");
        break;
      } else {
        printf("error.\n");
        close(s);
      }
    }
  }
  if (res == NULL) {
    printf("\n");
    fprintf(stderr, "socket or connect error\n");
    exit(EXIT_FAILURE);
  }

  /* 
   * ���饤����Ƚ����ᥤ��롼����
   */
  while (1) {
    char cmd[BUFSIZE];       /* ���ϥ��ޥ�ɤθ����� */
    char recv_buf[BUFSIZE];  /* �����Хåե�         */

    /*
     * ���ޥ�����Ͻ�������������
     */
    printf("TCP>");
    fflush(stdout);

    /* ���ޥ�ɤ����� */
    if (fgets(send_buf, BUFSIZE - 2, stdin) == NULL)
      break;
    cmd[0] = '\0';
    sscanf(send_buf, "%s", cmd);
    if (strcmp(cmd, "quit") == 0)
      break;
    if (strcmp(cmd, "") == 0)
      strcpy(send_buf, "help\n");

#ifdef HTTP
    strncat(send_buf, "Connection: keep-alive\n\n", BUFSIZE
                                       - strlen(send_buf) - 1);
#endif

    /* ���ޥ�ɤ����� */
    if (send(s, send_buf, strlen(send_buf), 0) <= 0) {
      perror("send");
      break;
    }

    /*
     * ���ץꥱ�������إå��μ���������
     */
    len = -1;
    while (1) {
      char *cmd1;  /* ���ޥ�ɤ�1����� */
      char *cmd2;  /* ���ޥ�ɤ�2����� */
      int i;       /* ����ʸ���Υ������  */

      /* ���ȥ꡼�෿�Υǡ����μ������� */
      for (i = 0; i < BUFSIZE - 1; i++) {
        if (recv(s, &recv_buf[i], 1, 0) <= 0)
          goto exit_loop;
        if (recv_buf[i] == '\n')  /* ����ñ�̤Ǽ��������򤹤� */
          break;
      }
      if (i >= 1 && recv_buf[i - 1] == '\r')  /* ���Ԥ�CR��LF�λ��ν��� */
        i--;
      if (i == 0)  /* ���Ԥξ��ϡ��إå�������ꡢ�ǡ��������Ϥޤ� */
        break;

      /* �إå��β��Ͻ��� */
      recv_buf[i] = '\0';
      cmd1 = strtok(recv_buf, ": ");
      cmd2 = strtok(NULL, " \0");
#ifdef DEBUG
      printf("[%s, %s]\n", cmd1, cmd2);
#endif
      if (strcmp("Content-Length", cmd1) == 0)
        len = atoi(cmd2);
    }

#ifdef HTTP
    if (len == -1) {
      while ((n = recv(s, recv_buf, BUFSIZE - 1, 0)) > 0) {
        recv_buf[n] = '\0';
        printf("%s", recv_buf);
        fflush(stdout);
      }
      close(s);
      return 0;
    }
#endif

    /*
     * ���ץꥱ�������ǡ����μ��������̤ؤν���
     */
    while (len > 0) {
      if ((n = recv(s, recv_buf, BUFSIZE - 1, 0)) <= 0)
        goto exit_loop;
      recv_buf[n] = '\0';
      len -= n;
      printf("%s", recv_buf);
      fflush(stdout);
    }
  }
 exit_loop:
  n = snprintf(send_buf, BUFSIZE, "quit\n");
  send(s, send_buf, n, 0);
  close(s);
  freeaddrinfo(res0);
  printf("connection closed.\n");

  return 0;
}
