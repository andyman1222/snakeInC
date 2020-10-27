#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

char i, dir;
int yMax, xMax;

typedef struct items{
	int x, y;
}item;

item item1;

//key listener. dir is just a boolean specifying direction (no duh)
DWORD WINAPI keyThread() {
	noecho();
	raw();
	while (1) {
		i = getch();
	}
}

//move the random item item1 to a random pos on the screen (no duh)
void randomPos() {
	item1.x = rand()%xMax;
	item1.y = rand()%yMax;
}

int main() {
	srand(time(NULL));
	initscr();
	curs_set(FALSE);
	CreateThread(NULL, 0, keyThread, NULL, 0, NULL);
	getmaxyx(stdscr, yMax, xMax);
	int xPos = 1, yPos = 1;
	randomPos();
	char dir = 'L';
	while (1) {
		//DO ALL 'DRAWING' BETWEEN CLEAR() AND REFRESH()
		clear();
		mvprintw(yPos, xPos, "o");
		mvprintw(item1.y, item1.x, ".");
		refresh();
		Sleep(50);
		switch (i) {
		case 'w':
			dir = 'U';
			break;
		case 's':
			dir = 'D';
			break;
		case 'a':
			dir = 'L';
			break;
		case 'd':
			dir = 'R';
			break;
		}
		switch (dir) {
		case 'U':
			if (yPos <= 0) yPos = yMax - 1;
			else yPos--;
			break;
		case 'D':
			if (yPos >= yMax - 1) yPos = 0;
			else yPos++;
			break;
		case 'L':
			if (xPos <= 0) xPos = xMax - 1;
			else xPos--;
			break;
		case 'R':
			if (xPos >= xMax - 1) xPos = 0;
			else xPos++;
			break;
		}

		//simply snake- if the o touches the ., it will eat the ..
		if (yPos == item1.y && xPos == item1.x) randomPos();
	}
	return 0;
}


