#define clear_screen puts("\x1B[2J");
#define gotoxy(x, y) printf("\033[%d;%dH", x, y)
