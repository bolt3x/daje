#include <stdio.h>
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
	struct timeval tv;
	int timeout = 100;
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
			printf("Inviato il segnale SIGKILL");
		}
			
		if(signal == 'T'){
			kill(pid,SIGTERM);
			printf("Inviato il segnale SIGTERM");
		}
		
		clear_lines(0,0,1);
	}

	return 0;
}
