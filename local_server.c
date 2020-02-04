#include "utils.h"
int initUdpReceiver();
void handler(int signo){};
void sendMessageToIP(uint32_t remoteIP, int localId, int senderSocket, struct sockaddr_in *destAddr, MsgBuf *msg, int msgSize);
void sendMessageToQueue(int queueId, MsgBuf *msg, int msgSize);
/*
*  2 message queue from now on
*  One for sending messages to hosts in other galaxy (Remote queue)
*  One for sending messages to hosts in own galaxy (Local queue)
*  Queue id will be decided by port number of the local server(fixed)
*/
int main()
{
	// My port number
	int myPort = UDP_PORT;
	int someRandomNum = 17;
	char somePath[someRandomNum];
	sprintf(somePath, "%d", myPort);
	key_t localQueue = ftok(somePath, someRandomNum);
	key_t remoteQueue = ftok(somePath, someRandomNum << 1);

	int localId = msgget(localQueue, IPC_CREAT | 0644);
	if (localId < 0)
		die("mesget failed");
	int remoteId = msgget(remoteQueue, IPC_CREAT | 0644);
	if (remoteId < 0)
		die("mesget failed");

	//No need of creating child process as the server would be single threaded (Non-blocking IO)

	// Initiating UDP receiver
	int listenerSocket = initUDPListener();
	int senderSocket = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
	if (senderSocket == -1)
		die("Error Sender Socket Creation");

	MsgBuf msg;
	uint32_t remoteIP;
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(UDP_PORT);
	localIP = getLocalIP();
	int waitTimeInSec = 3;
	signal(SIGALRM, handler);
	int msgSize;
	while (1)
	{
		// Message receive subRoutine for getting message from remoteQueue
		msgSize = msgrcv(remoteId, &msg, sizeof(msg), 0, IPC_NOWAIT);
		if (msgSize != -1)
		{
			remoteIP = extractIP(msg.mtext);
			sendMessageToIP(remoteIP, localId, senderSocket, &destAddr, &msg, msgSize);
		}
		msgSize = listenOnSocket(listenerSocket, &msg, waitTimeInSec);
		// Send to local process
		if (msgSize != -1)
			sendMessageToQueue(localId, &msg, msgSize);
	}
	return 0;
}

/*
* Initiating UDP Receive Socket
*/
int initUdpReceiver()
{
	// 	printf("udp_receiver(%d)\n", msgid);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in recvaddr;
	recvaddr.sin_family = AF_INET;
	recvaddr.sin_addr.s_addr = INADDR_ANY;
	recvaddr.sin_port = htons(UDP_PORT);
	// error block for bind
	bind(sock, (struct sockaddr *)&recvaddr, sizeof(recvaddr));
	// success ->
	// printf("UDP listener created on  %d\n",UDP_PORT);
	return sock;
}

/*
* Listener Routine
*/
int listenOnSocket(int sock, MsgBuf *msg, int waitTime)
{
	// Timer for waittime
	alarm(waitTime);
	ssize_t t = recv(sock, msg, sizeof(msg), 0);
	alarm(0);
	return t;
}
/*
* Send message to IP
*/
void sendMessageToIP(uint32_t remoteIP, int localId, int senderSocket, struct sockaddr_in *destAddr, MsgBuf *msg, int msgSize)
{
	if (remoteIP == localIP)
		sendMessageToQueue(localId, msg, msgSize);
	else
	{
		destAddr->sin_addr.s_addr = remoteIP;
		if (sendto(senderSocket, msg, msgSize, 0, (struct sockaddr *)destAddr, sizeof(*destAddr)) == -1)
			die("Failed to Send to the destination");
	}
	return;
}
/*
* Send message to message queue
*/
void sendMessageToQueue(int queueId, MsgBuf *msg, int msgSize)
{
	if (msgsnd(queueId, msg, msgSize, IPC_NOWAIT) == -1)
		die("Failed to send in local Message Queue");
	return;
}