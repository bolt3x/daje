#include <stdlib.h>              
#include <string.h>
#include <signal.h>
#include <stdio.h> 
#include <unistd.h>
#include <poll.h>
#include "terminal_control.h"
#include "read.h"
#include <sys/ioctl.h>
#include <unistd.h>

struct winsize w;

int win_change_handler(){
	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	clear_screen();
	return 0;
}

void quit_handler(){
	
	clear_screen();
	
	echo_on();
	
	exit(EXIT_SUCCESS);
}
//bolt3x 
//helper function that clears n lines starting from (x,y)
void clear_lines(int x,int y,int n){
	
	gotoxy(y,x);
	
	for(int i = 0; i < n; i++){
		for(int j = 0; j < 100; j++)
			printf(" ");
		printf("\n");
	}

}


//bolt3x
//function that sends the signal selected by the user
int signal_sender(){
	
	int pid;
	char signal;
	int retval;
	struct timeval tv;
	int timeout = 5000;
	struct pollfd fd;
	fd.fd = 0;
	fd.events = POLLIN;
	fd.revents = 0;
	
	
	gotoxy(0,0);
	echo_off();
	printf("Premere ENTER per inviare un sengale");
	fflush(NULL);
	int ret = poll(&fd, 1, timeout);
	if (ret > 0 && (fd.revents & POLLIN != 0))  {
		clear_lines(0,0,1);
		gotoxy(0,0);
		echo_on();
		printf("Indicare il pid del processo a cui inviare il segnale: ");
		scanf("%d",&pid);
		clear_lines(0,0,1);
		gotoxy(0,0);	
		printf("Selezionare il segnale da inviare: K per SIGKILL: ");
		scanf(" %c",&signal);
		clear_lines(0,0,1);
		gotoxy(0,0);	
		if(signal == 'K'){
			kill(pid,SIGKILL);
			printf("Inviato il segnale SIGKILL\n");
		}
			
		if(signal == 'T'){
			kill(pid,SIGTERM);
			printf("Inviato il segnale SIGTERM\n");
		}
		
		clear_lines(0,0,3);
	}

	return 0;
}

int main() {
	
	signal(SIGWINCH,win_change_handler);
	
	signal(SIGINT,quit_handler);
	
	clear_screen();
	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	while(1){
		
		
		myRead(w.ws_row);
		signal_sender();
		clear_lines(4,0,w.ws_row);
	}
	

	return 0;
}
