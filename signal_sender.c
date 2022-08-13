#include <stdio.h>
#include <signal.h>

#define gotoxy(x, y) printf("\033[%d;%dH", x, y)

//bolt3x 
//helper function that clears n lines starting from (x,y)

void clear_lines(int x,int y,int n){
	
	gotoxy(x,y);
	
	for(int i = 0; i < n; i++){
		for(int j = 0; j < 100; j++)
			printf(" ");
		printf("\n");
	}
	
	gotoxy(x,y);
}


//bolt3x
//function that sends the signal selected by the user

int signal_sender(){
	int pid;
	char signal;
	
	while(1){
		printf("Indicare il pid del processo a cui inviare il segnale: ");
		scanf("%d",&pid);
		
		printf("Selezionare il segnale da inviare: K per SIGKILL: ");
		scanf(" %c",&signal);
		
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
