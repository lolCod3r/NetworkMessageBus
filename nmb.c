#include "util.h"

void handleError(char *s){
	perror(s);
	exit(0);
}

int msgget_nmb(){
	int key = ftok("./queue",projectId);
	int queueId;
	if((queueId = msgget(key, IPC_CREAT)) == -1)
		handleError("Queue");
	return queueId;		
}

int msgsnd_nmb(int queueId, char *msg, int msgLength, int pid, char *ip, int flags){
	struct queuebuf qbuf;
	qbuf.mtype = listenerPort;
	struct msgbuf *buf = (struct msgbuf*)qbuf.mtext;
	buf->ip = inet_addr(ip);
	buf->pid = pid;
	if(msgLength < MAXBUFLEN)
		msg[msgLength] = '\0';
	else{
		msgLength = MAXBUFLEN;
		msg[msgLength-1] = '\0';
	}
	strcpy(buf->msg, msg);
	if(msgsnd(queueId, &qbuf, 4+2+msgLength, 0) == -1)
		return -1;
	return 0;
}

int msgrcv_nmb(int queueId, char *msg, int msgLength, int pid, int flags){
	struct queuebuf qbuf;
	int rcvSize;
	if((rcvSize = msgrcv(queueId, &qbuf, sizeof(struct msgbuf), (long)pid, 0)) == -1)
		return -1;
	strcpy(msg,((struct msgbuf *)qbuf.mtext)->msg);
	return rcvSize;
}