#ifndef __NMB_UTIL
#define __NMB_UTIL
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/stat.h>

#define listenerPort 8000
#define projectId 1234
#define myIPaddr "0.0.0.0"
#define MAXBUFLEN 200

struct msgbuf{
	unsigned int ip;
	unsigned short int pid;
	char msg[MAXBUFLEN];
};

struct queuebuf{
	long mtype; // Local use of port
	char mtext[sizeof(struct msgbuf)];
};

void handleError(char *s);
int msgget_nmb();
int msgsnd_nmb(int queueId, char *msg, int msgLength, int pid, char *ip, int flags);
int msgrcv_nmb(int queueId, char *msg, int msgLength, int pid, int flags);

#endif