#pragma once

#include <termios.h>

#define clear_screen() puts("\x1B[2J")

#define gotoxy(x, y) printf("\033[%d;%dH", x, y)

#define RED     "\033[31m"

#define RESET   "\033[0m"

void echo_off();
void echo_on();

