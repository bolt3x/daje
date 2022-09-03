#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ioctl.h>

#include "signal_sender.h"
#include "terminal_control.h"
#include "read.h"

struct winsize w;


int win_change_handler(){
	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	clear_screen();
	
	return 0;
}

void quit_handler(){
	
	clear_screen();
	
	echo_on();
	
	gotoxy(0,0);
	
	exit(EXIT_SUCCESS);
}

int main() {
	
	signal(SIGWINCH,win_change_handler);
	
	signal(SIGINT,quit_handler);
	
	clear_screen();
	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	
	while(1){
		
		myRead(w.ws_row);
		
		signal_sender();
		
		clear_lines(1,0,w.ws_row);
		
	}
	

	return 0;
}
