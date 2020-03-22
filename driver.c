#include "util.h"

enum states{SEND=1, RCV, PRINT_DETAILS, SHOW_MENU, QUIT, WAIT};

void print_banner();

int main(int argc, char* argv[]){
	
	print_banner();
	int nmbid = msgget_nmb();

	enum states state = SHOW_MENU;
	char ip[INET_ADDRSTRLEN], text[MAXBUFLEN];
	int pid = -1,myPid = getpid();
	printf("My pid: %d\n", myPid);

	while(1){
		printf("     __________________________     \n\n");
		switch(state){
			case SHOW_MENU:
				printf("Choose an option:\n");
				printf("1 Send a message\n");
				printf("2 Read a message\n");
				printf("3 Print Details\n");
				printf("4 Show menu\n");
				printf("5 Quit\n");
				state = WAIT;
				break;
			case SEND:
				printf("Enter IP:\n");
				scanf("%s", ip);
				printf("Enter pid:\n");
				scanf("%d", &pid);
				fgetc(stdin);
				printf("Enter message:\n");
				fgets(text, sizeof(text), stdin);
				printf("Sending message...\n");
				if(msgsnd_nmb(nmbid, text, sizeof(text),pid, ip,0) < 0)
					handleError("msgsend");
				printf("Message sent...\n");
				state = SHOW_MENU;
				break;
			case RCV:
				printf("Reading a message...\n");
				if(msgrcv_nmb(nmbid, text, MAXBUFLEN, myPid, 0) < 0)
					handleError("msgrecv");
				printf("Here you go:\n");
				printf("%s\n", text);
				state = SHOW_MENU;
				break;
			case PRINT_DETAILS:
				
				printf("My pid: %d\n", myPid);
				state = SHOW_MENU;
			case WAIT:
				scanf("%d", (int *)&state);
				break;
			case QUIT:
				printf("Bye Bye...\n");
				exit(0);
			default:
				printf("Bad choice, try again...\n");
				state = SHOW_MENU;
		}
	}
	return 0;
}


void print_banner(){
	// tadaaa
	printf("\n");
	printf("      **  **   **   **   ****       \n");
	printf("      *** **   **   **   ** **      \n");
	printf("      ******   *** ***   ****       \n");
	printf("      ** ***   ** * **   ** **      \n");
	printf("      **  ** * **   ** * ****       \n");
	printf("     __________________________     \n");
	printf("\n");

}	
