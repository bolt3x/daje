#pragma once

#include <termios.h>
#include <stdio.h>

#define clear_screen() puts("\x1B[2J")

#define gotoxy(x,y) printf("\033[%d;%dH", y, x)

#define RED     "\033[31m"

#define RESET   "\033[0m"



void echo_off();
void echo_on();
void clear_lines(int x,int y,int n);
