#include "util.h"

void handleError(char *s){
	perror(s);
	exit(0);
};


int createUDPlistener(){
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		handleError("Socket");
	struct sockaddr_in myAddr;
	memset(&myAddr, 0 , sizeof(struct sockaddr_in));
	myAddr.sin_family = AF_INET;
	myAddr.sin_addr.s_addr = INADDR_ANY;
	myAddr.sin_port = htons(listenerPort);

	if(bind(sock, (struct sockaddr *)&myAddr, sizeof(struct sockaddr_in)) == -1)
		handleError("bind");
	return sock;
}

int createUDPsender(){
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == -1)
		handleError("Socket");
	return sock;	
}

int initializeQueue(){
	key_t key = ftok("./queue",projectId);
	int q = msgget(key, IPC_CREAT| S_IRUSR| S_IWUSR);
	if(q == -1)
		handleError("msgget");
	return q;
}	

void listenerRoutine(int listenerFd, int queueId){
	struct queuebuf qbuf;
	printf("Starting UDP server\n");
	while(recv(listenerFd, qbuf.mtext,sizeof(struct msgbuf), 0) != -1){
		qbuf.mtype = (long)((struct msgbuf*)qbuf.mtext)->pid;
		if(msgsnd(queueId, &qbuf, sizeof(struct msgbuf), 0) == -1)
			handleError("MsgSend");
	}
	handleError("recv");
}

void senderRoutine(int senderFd, int queueId){
	struct queuebuf qbuf;
	struct sockaddr_in destAddr;
	memset(&destAddr, 0 , sizeof(struct sockaddr_in));
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(listenerPort);
	printf("Waiting for msg to send to UDP servers\n");
	while(msgrcv(queueId, &qbuf, sizeof(struct msgbuf), listenerPort, 0) != -1){
		destAddr.sin_addr.s_addr = ((struct msgbuf*)qbuf.mtext)->ip;
		if(sendto(senderFd, qbuf.mtext, sizeof(struct msgbuf), 0, (struct sockaddr *)&destAddr, sizeof(destAddr)) == -1)
			handleError("Send");
	}
	handleError("MsgRcv");
}

int main(){
	int process;
	int q = initializeQueue();
	process = fork();
	if(process > 0){
		int listener = createUDPlistener();
		listenerRoutine(listener,q);
		exit(0);
	}
	int sender = createUDPsender();
	senderRoutine(sender,q);
	return 0;
}