/**********************************************************************
 *  UDP����ץ륵���� (udps.c)
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

#define BUFSIZE      32768
#define MSGSIZE      1024
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

enum {CMD_NAME, DST_PORT};

int execute(char *command, char *buf, int bufmax);

int main(int argc, char *argv[])
{
  struct sockaddr_in server;  /* �����ФΥ��ɥ쥹           */
  struct sockaddr_in client;  /* ���饤����ȤΥ��ɥ쥹     */
  unsigned int len;           /* sockaddr_in��Ĺ��          */
  int port;                   /* �����ФΥݡ����ֹ�         */
  int s;                      /* �����åȥǥ�������ץ�     */
  int cn;                     /* �����������ޥ�ɤΥ�ɿ� */
  int sn;                     /* ������å������ΥХ��ȿ�   */
  int rn;                     /* ������å������ΥХ��ȿ�   */
  int i;                      /* �롼���ѿ�                 */
  char cmd1[BUFSIZE];         /* 1����ܤΥ��ޥ��        */
  char cmd2[BUFSIZE];         /* 2����ܤΥ��ޥ��        */
  char recv_buf[BUFSIZE];     /* �����Хåե�               */
  char send_buf[BUFSIZE];     /* �����Хåե�               */

  /* �������ν���(�ݡ����ֹ�) */
  if (argc == 2) {
    if ((port = atoi(argv[DST_PORT])) == 0) {
      struct servent *se;     /* �����ӥ����� */

      if ((se = getservbyname(argv[DST_PORT], "udp")) != NULL)
        port = (int) ntohs((u_int16_t) se->s_port);
      else {
        fprintf(stderr, "getservbyname error\n");
        exit(EXIT_FAILURE);
      }
    }
  } else
    port = DEFAULT_PORT;

  /* UDP�ǥ����åȤ򥪡��ץ󤹤� */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  /* �����ФΥ��ɥ쥹�����ꤹ�� */
  memset(&server, 0, sizeof server);
  server.sin_family        = AF_INET;
  server.sin_addr.s_addr   = htonl(INADDR_ANY);
  server.sin_port          = htons(port);
  if (bind(s, (struct sockaddr *) &server, sizeof server) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  /* �����н����ᥤ��롼���� */
  while ((rn = recvfrom(s, recv_buf, BUFSIZE - 1, 0,
                        (struct sockaddr *) &client,
                        (len = sizeof client, &len))) >= 0) {
    recv_buf[rn] = '\0';
    printf("receive from '%s'\n", inet_ntoa(client.sin_addr));
    printf("receive data '%s'\n", recv_buf);

    /* �������ޥ�ɤν��� */
    if ((cn = sscanf(recv_buf, "%s%s", cmd1, cmd2)) <= 0)
      sn = 0; /* continue; */
    else if (cn == 2 && strcmp(cmd1, "show") == 0) {
      if (strcmp(cmd2, "route") == 0)
        sn = execute(COMMAND_ROUTE, send_buf, BUFSIZE);
      else if (strcmp(cmd2, "arp") == 0)
        sn = execute(COMMAND_ARP, send_buf, BUFSIZE);
      else if (strcmp(cmd2, "tcp") == 0)
        sn = execute(COMMAND_TCP, send_buf, BUFSIZE);
      else if (strcmp(cmd2, "nic") == 0)
        sn = execute(COMMAND_NIC, send_buf, BUFSIZE);
      else
        sn = snprintf(send_buf, BUFSIZE, "parameter error '%s'\n"
                                         "show [route|arp|tcp|nic]\n", cmd2);
    } else {
      if (strcmp(cmd1, "quit") == 0)
        break;

      send_buf[0] = '\0';
      if (cn != 1 && strcmp(cmd1, "help") != 0)
        snprintf(send_buf, BUFSIZE, "command error '%s'\n", cmd1);
      strncat(send_buf, "Command:\n"
                        "  show route\n"
                        "  show arp\n"
                        "  show tcp\n"
                        "  show nic\n"
                        "  quit\n"
                        "  help\n", BUFSIZE - strlen(send_buf) - 1);
      sn = strlen(send_buf);
    }

    /* ��å����������롼�� (�Ǹ��ɬ��MSGSIZE̤���Υѥ��åȤ���������) */
    for (i = 0; i <= sn; i += MSGSIZE) {
      int size;  /* ���������å������Υ����� */

      if (sn - i > MSGSIZE)
        size = MSGSIZE;
      else if (sn - i <= 0)
        size = 0;
      else
        size = sn - i;
      if (sendto(s,&send_buf[i],size,0,(struct sockaddr *)&client,len) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
      }
    }
    printf("%s", send_buf);
  }
  close(s);

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
