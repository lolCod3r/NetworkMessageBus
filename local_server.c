#include "utils.h"
void udp_listener(int msgid);
int send_msg_to_udp(MsgBuf msg);
void create_tcp(int msgq);
void handle_tcp(int connfd, int msgq);
int put_in_msg_queue(int msgid, MsgBuf buf);
int retrieve_msg_queue(int msgq, long type, MsgBuf* msgp);

/*
*  2 message queue from now on
*  One for sending messages to hosts in other galaxy (Remote queue)
*  One for sending messages to hosts in own galaxy (Local queue)
*  Queue id will be decided by port number of the local server(fixed)
*/

/*
* Since Sys V message queue does not give file descriptors for Event-Driven IO
* We are taking our own time as 1 sec for not using more CPU cycles.
* 
*/

// Need to Decide on message


int main(){
	// My port number
	int myPort = ;
	int someRandomNum = 17;
	char somePath[someRandomNum];
	sprintf(somePath,"%d",myPort);
	key_t localQueue = ftok(somePath,someRandomNumber);
	key_t remoteQueue = ftok(somePath,someRandomNumber<<1);
	
	int localId = msgget (localQueue, IPC_CREAT | 0644);
	if(localId <0){ die("mesget failed");}
	int remoteId = msgget(remoteQueue, IPC_CREAT | 0644);
	if(remoteId < 0) { die("mesget failed");}
	
	
	//No need of creating child process as the server would be single threaded (Non-blocking IO)
	
	// Initiating UDP receiver 
	int listenerSocket = initUDPListener();
	int senderSocket = socket(AF_INET,SOCK_DGRAM|SOCK_NONBLOCK,0);
	if(senderSocket == -1){
		die("Error Sender Socket Creation");
	}
	MsgBuf msg;
	uint32_t remoteIP, localIP;
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(UDP_PORT);
	localIP = getLocalIP();
	
	while(1){
		// Message receive subRoutine for getting message from remoteQueue
		if(msgrcv(remoteId,&msg,sizeof(msg),0,IPC_NOWAIT|MSG_NOERROR) != -1){
			remoteIP = extractIp(msg.mtext);
			if(remoteIP == localIP){
				if(msgsnd(localId, &msg, sizeof(msg),IPC_NOWAIT) == -1){
					die("Failed to send in local Message Queue");
				}
			}
			else{
				sendAddr.sin_addr.s_addr = remoteIP;
				if(sendto(senderSocket, msg, sizeof(msg),0,(struct sockaddr *)&destAddr,sizeof(destAddr)) == -1){
					// If failed to recv send again 
					die("Failed to Send to the destination");
				}
			}
		/*
		* Remove message from remote queue
		*/
		};
		// Listen to message coming from other galaxies
		// recv for # seconds.
	}
	
	return 0;
}

int put_in_msg_queue(int msgid, MsgBuf buf){
	return msgsnd(msgid, &(buf.mtype), sizeof(buf.mtext), 0);
}

int retrieve_msg_queue(int msgq, long type, MsgBuf* msgp){
	return msgrcv(msgq, msgp, sizeof(msgp->mtext), type, 0);
}
/*
* Initiating UDP Receive Socket
*/
int initUdpReceiver(){
// 	printf("udp_receiver(%d)\n", msgid);
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in recvaddr;
	recvaddr.sin_family = AF_INET;
	recvaddr.sin_addr.s_addr = INADDR_ANY;
	recvaddr.sin_port = htons(UDP_PORT);
	// error block for bind
	bind(sock,(struct sockaddr *)&recvaddr,sizeof(recvaddr));
	// success ->
	// printf("UDP listener created on  %d\n",UDP_PORT);
	return sock;
}

/*
{
	for (;;) { 
		MsgBuf buf;
		ssize_t t = recv(sock, &buf, sizeof(buf), 0);
		if (t < 0)
			die("recv() failed");
		if(t == 0){
			break;		
		}
		// put in queue
		printf("mtext: %s\n", buf.mtext);
		put_in_msg_queue(msgid, buf);
	}
	close(sock);
	return;
}
*/
// No need of a TCP client
/*
void create_tcp(int msgq){
	printf("create_tcp(%d)\n", msgq);
	int connfd, sock;
	struct sockaddr_in listaddr, cliaddr;
	pid_t pid;
	unsigned int clilen;

	sock = socket(AF_INET,SOCK_STREAM,0);
	listaddr.sin_family = AF_INET;
	listaddr.sin_addr.s_addr = INADDR_ANY;
	listaddr.sin_port = htons(TCP_PORT);
	bind(sock,(struct sockaddr *)&listaddr,sizeof(listaddr));
	listen(sock,5);
	
	for(;;){
		clilen = sizeof (cliaddr);
		connfd = accept (sock, (struct sockaddr *) &cliaddr, &clilen);
		if(connfd< 0)die("accept() failed");
		
		if((pid =fork()) == 0){
			close(sock);
			handle_tcp(connfd, msgq);
			exit(0);
		}
		else if (pid < 0){die("fork() failed");}
		close(connfd);
	}
	close(sock);
}
*/
// In code handling
/*
int send_msg_to_udp(MsgBuf msg){
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in sendaddr;
	sendaddr.sin_family = AF_INET;
	int p;
	extract(msg.mtype, &(sendaddr.sin_addr.s_addr), &p);
	sendaddr.sin_port = htons(UDP_PORT);
	msg.mtype = (long)p;
	return sendto(sock, &(msg.mtype), sizeof(msg), 0, (struct sockaddr *)&sendaddr,sizeof(sendaddr));
}
*/

//No Need of TCP handler
/*
void handle_tcp(int connfd, int msgq){
	TcpCall tc;
	int t, action;
	long type;
	MsgBuf msg;

	for(;;){
		t = recv(connfd, &tc, sizeof(tc), 0);
		
		if(t < 0) die("recv() failed");
		if(t == 0) break; // connection closed
			
		// messnd_nmb
		action = tc.action;
		switch(action){
			case MSGSND:
				send_msg_to_udp(tc.msg);
				break;
			case MSGRCV:
				// check if msg is there
				type = tc.msg.mtype;
				t = retrieve_msg_queue(msgq, type, &msg);
			
				if( t < 0 ){
					die("retrieve_msg_queue >> msgrcv() failed");
				}
				send(connfd,&(msg.mtype),sizeof(msg), 0);
				break;
			default:
				printf("wrong action type\n");
		}
	}
	close (connfd);
	exit(0);
}
*/







