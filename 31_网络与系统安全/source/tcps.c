/**********************************************************************
 *  TCP����ץ륵���Хץ���� (tcps.c)
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
#ifdef FORK_SERVER
#include <signal.h>
#include <sys/wait.h>
#endif

#define BUFSIZE      8192
#define BUFSIZE2     32768
#define DEFAULT_PORT 5320

/* �롼�ƥ��󥰥ơ��֥��ɽ�������륳�ޥ�� */
#ifdef __linux
#define COMMAND_ROUTE "/bin/netstat -rn"
#else
#define COMMAND_ROUTE "netstat -rn"
#endif

/* ARP�ơ��֥��ɽ�������륳�ޥ�� */
#ifdef __linux
#define COMMAND_ARP "/sbin/arp -an"
#else
#define COMMAND_ARP "arp -an"
#endif

/* TCP���ͥ������ơ��֥��ɽ�������륳�ޥ�� */
#ifdef __linux
#define COMMAND_TCP "/bin/netstat -tn"
#else
#define COMMAND_TCP "netstat -n"
#endif

/* NIC�ξ����ɽ�������륳�ޥ�� */
#ifdef __linux
#define COMMAND_NIC "/sbin/ifconfig -a"
#else
#define COMMAND_NIC "ifconfig -a"
#endif

enum {CMD_NAME, SRC_PORT};

int execute(char *command, char *buf, int bufmax);

#ifdef FORK_SERVER
void reaper()
{
  int status;
  wait(&status);
}
#endif

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* �����ФΥ��ɥ쥹                     */
  struct sockaddr_in client;  /* ���饤����ȤΥ��ɥ쥹               */
  unsigned int len;           /* sockaddr_in��Ĺ��                    */
  int port;                   /* �����ФΥݡ����ֹ�                   */
  int s;                      /* ��å������������ѥǥ�������ץ�     */
  int s0;                     /* ���ͥ����������դ��ѥǥ�������ץ� */

  /* �������ν���(�ݡ����ֹ�) */
  if (argc == 2) {
    if ((port = atoi(argv[SRC_PORT])) == 0) {
      struct servent *se;  /* �����ӥ����� */

      if ((se = getservbyname(argv[SRC_PORT], "tcp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* TCP�ǥ����åȤ򥪡��ץ󤹤� */
  if ((s0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* �����ФΥ��ɥ쥹�����ꤹ�� */
  memset(&server, 0, sizeof server);
  server.sin_family      = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port        = htons(port);
  if (bind(s0, (struct sockaddr *) &server, sizeof server) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* ���ͥ�������Ω�׵���ճ��� */
  if (listen(s0, 5) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

#ifdef FORK_SERVER
  signal(SIGCHLD, (void *) reaper);
#endif

  /*
   * ���ͥ����������դ��롼�� 
   */
  while (1) {
    /* ���ͥ��������ս��� */
    len = sizeof client;
    if ((s = accept(s0, (struct sockaddr *) &client, &len)) < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }
    /* ��³�������饤����Ȥ�IP���ɥ쥹��ɽ�� */
    printf("connected from '%s'\n", inet_ntoa(client.sin_addr));

#ifdef FORK_SERVER
    if (fork() != 0) {
      close(s);
      continue;
    }
    close(s0);
#endif

    /* �����н����ᥤ��롼���� */
    while (1) {
      char recv_buf[BUFSIZE];    /* �����Хåե�               */
      char cmd1[BUFSIZE];        /* 1����ܤΥ��ޥ��        */
      char cmd2[BUFSIZE];        /* 2����ܤΥ��ޥ��        */
      int cn;                    /* �����������ޥ�ɤΥ�ɿ� */
      int i;                     /* ����ʸ���Υ������         */
      char send_head[BUFSIZE];   /* �����ǡ����Υإå�         */
      char send_data[BUFSIZE2];  /* �����Хåե�               */
      int hn;                    /* �����إå��ΥХ��ȿ�       */
      int dn;                    /* �����ǡ����ΥХ��ȿ�       */

      /* ���ޥ�ɤμ��� */
      recv_buf[0] = '\0';
      for (i = 0; i < BUFSIZE - 1; i++) {
        if (recv(s, &recv_buf[i], 1, 0) <= 0)
          goto exit_loop;
        if (recv_buf[i] == '\n')  /* ����ñ�̤Ǽ��������򤹤� */
          break;
      }
      recv_buf[i] = '\0';
      printf("receive '%s'\n", recv_buf);

      /* �������ޥ�ɤν��� */
      if ((cn = sscanf(recv_buf, "%s%s", cmd1, cmd2)) <= 0)
        continue;
      if (cn == 2 && strcmp(cmd1, "show") == 0) {
        if (strcmp(cmd2, "route") == 0)
          dn = execute(COMMAND_ROUTE, send_data, BUFSIZE2);
        else if (strcmp(cmd2, "arp") == 0) 
          dn = execute(COMMAND_ARP, send_data, BUFSIZE2);
        else if (strcmp(cmd2, "tcp") == 0)
          dn = execute(COMMAND_TCP, send_data, BUFSIZE2);
        else if (strcmp(cmd2, "nic") == 0)
          dn = execute(COMMAND_NIC, send_data, BUFSIZE2);
        else
          dn = snprintf(send_data, BUFSIZE2, "parameter error '%s'\n"
                                           "show [route|arp|tcp|nic]\n", cmd2);
      } else {
        if (strcmp(cmd1, "quit") == 0)
          goto exit_loop;

        send_data[0] = '\0';
        if (cn != 1 && strcmp(cmd1, "help") != 0)
          snprintf(send_data, BUFSIZE2 - 1, "command error '%s'\n", cmd1);
        strncat(send_data, "Command:\n"
                           "  show route\n"
                           "  show arp\n"
                           "  show tcp\n"
                           "  show nic\n"
                           "  quit\n"
                           "  help\n", BUFSIZE2 - strlen(send_data) - 1);
        dn = strlen(send_data);
      }

      /* ���ץꥱ�������إå������� */
      hn = snprintf(send_head, BUFSIZE - 1, "Content-Length: %d\n\n", dn);
      if (send(s, send_head, hn, 0) < 0)
        break;
      send_head[hn] = '\0';
      printf("%s\n", send_head);

      /* ���ץꥱ�������ǡ��������� */
      if (send(s, send_data, dn, 0) < 0)
        break;
      printf("%s\n", send_data);
    }
  exit_loop:
    printf("connection closed.\n");
    close(s);
#ifdef FORK_SERVER
    return 0;
#endif
  }
  close(s0);

  return 0;
}

/*
 * int execute(char *command, char *buf, int bufmax);
 *
 * ��ǽ
 *     ���ޥ�ɤ�¹Ԥ������Ϸ�̤�Хåե��˳�Ǽ����
 * ������ 
 *     char *command;  �¹Ԥ��륳�ޥ��
 *     char *buf;      ���Ϸ�̤��Ǽ����Хåե�
 *     int bufmax;     �Хåե����礭��
 * �����
 *     int             �Хåե��˳�Ǽ����ʸ����
 */
int execute(char *command, char *buf, int bufmax)
{
  FILE *fp;  /* �ե�����ݥ���         */
  int i;     /* ���Ϥ����ǡ����ΥХ��ȿ� */

  if ((fp = popen(command, "r")) == NULL) {
    perror(command);
    i = snprintf(buf, BUFSIZE, "server error: '%s' cannot execute.\n", command);
  } else {
    i = 0;
    while (i < bufmax - 1 && fscanf(fp, "%c", &buf[i]) == 1)
      i++;
    buf[i] = '\0';
    pclose(fp);
  }
  return i;
}
