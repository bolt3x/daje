#include <termios.h>
#include <stdio.h>

#include "terminal_control.h"

void echo_on(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag |= ECHO;
	tcsetattr(1, TCSANOW, &term);

}

void echo_off(){

	struct termios term;
	tcgetattr(1, &term);
	term.c_lflag &= ~ECHO;
	tcsetattr(1, TCSANOW, &term);

}

//bolt3x 
//helper function that clears n lines starting from (x,y)
void clear_lines(int x,int y,int n){
	
	gotoxy(y,x);
	
	for(int i = 0; i < n; i++){
		for(int j = 0; j < 100; j++)
			printf(" ");
		printf("");
	}

}
