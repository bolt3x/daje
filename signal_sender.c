#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <signal.h>
#include <sys/time.h>

#include "terminal_control.h"

//bolt3x
//function that sends the signal selected by the user
int signal_sender(){
	
	int pid;
	char signal;
	int retval;
	
	//valori per la poll
	struct timeval tv;
	int timeout = 1000;
	struct pollfd fd;
	fd.fd = 0;
	fd.events = POLLIN;
	fd.revents = 0;
	
	//aspetto il comando ENTER
	gotoxy(0,0);
	echo_off();
	printf("Premere ENTER per inviare un segnale ");
	fflush(NULL);
	int ret = 0;
	ret = poll(&fd, 1, timeout);
	
	if (ret > 0 && (fd.revents & POLLIN != 0))  {
		
		//aspetto il pid e ils segnale
		clear_lines(0,0,1);
		gotoxy(0,0);
		echo_on();
		printf("Indicare il pid del processo a cui inviare il segnale: ");
		scanf("%d",&pid);

		//aspetto e il segnale
		clear_lines(0,0,1);
		gotoxy(0,0);	
		printf("Selezionare il segnale da inviare: ");
		printf("\nK per SIGKILL,T per SIGTERM,S per SIGSTOP,C per SIGCONT");
		gotoxy(36,0);
		scanf(" %c",&signal);
		
		clear_lines(0,0,2);
		
		gotoxy(0,0);	

		if(signal == 'K'){
			kill(pid,SIGKILL);

		}
			
		if(signal == 'T'){
			kill(pid,SIGTERM);
		}				
		
		if(signal == 'S'){
			kill(pid,SIGSTOP);
		}

		if(signal == 'C'){
			kill(pid,SIGCONT);
		}
		
		clear_lines(0,0,2);
	}

	return 0;
}
