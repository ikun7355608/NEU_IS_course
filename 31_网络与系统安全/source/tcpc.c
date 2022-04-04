/*****************************************************************
 *  TCP����ץ륯�饤����ȥץ���� (tcpc.c)
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
 ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE      8192
#define DEFAULT_PORT 5320

enum {CMD_NAME, DST_IP, DST_PORT};

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* �����ФΥ��ɥ쥹             */
  unsigned long dst_ip;       /* �����Ф�IP���ɥ쥹           */
  int port;                   /* �ݡ����ֹ�                   */
  int s;                      /* �����åȥǥ�������ץ�       */
  int n;                      /* ���ϥǡ����ΥХ��ȿ�         */
  int len;                    /* ���ץꥱ�������ǡ�����Ĺ�� */
  char send_buf[BUFSIZE];     /* �����Хåե�                 */

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

      if ((se = getservbyname(argv[DST_PORT], "tcp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* TCP�ǥ����åȤ򳫤� */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* �����ФΥ��ɥ쥹�����ꤷ���ͥ��������Ω���� */
  memset(&server, 0, sizeof server);
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = dst_ip;
  server.sin_port        = htons(port);
  if (connect(s, (struct sockaddr *) &server, sizeof server) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  printf("connected to '%s'\n", inet_ntoa(server.sin_addr));

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

      /* ���ȥ꡼�෿��å������μ������� */
      for (i = 0; i < BUFSIZE - 1; i++) {
        if (recv(s, &recv_buf[i], 1, 0) <= 0)
          goto exit_loop;
        if (recv_buf[i] == '\n')  /* ����ñ�̤Ǽ��������򤹤� */
          break;
      }
      if (i >= 1 && recv_buf[i - 1] == '\r') /* ���Ԥ�CR��LF�λ��ν��� */
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

  return 0;
}
