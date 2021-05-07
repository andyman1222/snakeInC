#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>


int yMax, xMax;

struct snek {
	char i, dir;
	int* pointsX, *pointsY;
	int len;
	int xPos, yPos;
};

struct snek self;

typedef struct items{
	int x, y;
	int movable;
	
}item;
 
void newSnek(struct snek* s) {
	if (s->pointsX) free(s->pointsX);
	if (s->pointsY) free(s->pointsY);
	s->pointsX = calloc(2, sizeof(int));
	s->pointsY = calloc(2, sizeof(int));
	s->pointsX[0] = -1;
	s->pointsY[0] = -1;
	s->dir = 'L';
	s->xPos = s->yPos = 1;
	s->len = 1;
}

void addLength(struct snek* s) {
	s->len++;
	s->pointsX = realloc(s->pointsX, s->len * sizeof(int));
	s->pointsY = realloc(s->pointsY, s->len * sizeof(int));
	s->pointsX[s->len - 1] = -1;
	s->pointsY[s->len - 1] = -1;
}

void printSnek(struct snek* s) {
	mvprintw(s->yPos, s->xPos, "o");
	int prevX, prevY, prevPrevX, prevPrevY;
	for (int i = 0; i < s->len; i++) {
		if (i == 0) {
			prevX = s->xPos;
			prevY = s->yPos;
		}
		if (s->pointsX[i] >= 0 && s->pointsY[i] >= 0) {
			
			if (prevX == s->pointsX[i] && prevY == s->pointsY[i]) {

			}
			else if ((i == s->len-1 && prevX == s->pointsX[i]) || s->pointsX[i + 1] == prevX) {

				mvprintw(s->pointsY[i], s->pointsX[i], "|");
			}
			else if ((i == s->len - 1 && prevY == s->pointsY[i]) || s->pointsY[i + 1] == prevY) {
				mvprintw(s->pointsY[i], s->pointsX[i], "-");
			}
			else if (i < s->len-1 && (s->pointsY[i+1] - prevY) * (s->pointsX[i+1] - prevX) > 0) {
				mvprintw(s->pointsY[i], s->pointsX[i], "\\");
			}
			else if (i < s->len - 1 && (s->pointsY[i + 1] - prevY) * (s->pointsX[i + 1] - prevX) < 0) {
				mvprintw(s->pointsY[i], s->pointsX[i], "/");
			}
		}
		int tmp1 = s->pointsX[i];
		int tmp2 = s->pointsY[i];
		s->pointsX[i] = prevX;
		s->pointsY[i] = prevY;
		prevX = tmp1;
		prevY = tmp2;
		
	}
}

item *item1;

//key listener. dir is just a boolean specifying direction (no duh)
DWORD WINAPI keyThread() {
	noecho();
	raw();
	while (1) {
		self.i = getch();
	}
}

//move the random item item1 to a random pos on the screen (no duh)
void randomPos(struct items* i) {
	if (i->movable == 1) {
		i->x = rand() % xMax;
		i->y = rand() % yMax;
	}
	else free(i);
}

int main() {
	item1 = (item*)malloc(sizeof(struct items));
	item1->movable = 1;
	srand(time(NULL));
	initscr();
	curs_set(FALSE);
	CreateThread(NULL, 0, keyThread, NULL, 0, NULL);
	getmaxyx(stdscr, yMax, xMax);
	randomPos(item1);
	newSnek(&self);
	while (1) {
		//DO ALL 'DRAWING' BETWEEN CLEAR() AND REFRESH()
		clear();
		printSnek(&self);
		mvprintw(item1->y, item1->x, "=");
		refresh();
		Sleep(50);
		switch (self.i) {
		case 'w':
			if(self.dir != 'D')
				self.dir = 'U';
			break;
		case 's':
			if (self.dir != 'U')
				self.dir = 'D';
			break;
		case 'a':
			if (self.dir != 'R')
				self.dir = 'L';
			break;
		case 'd':
			if (self.dir != 'L')
				self.dir = 'R';
			break;
		}
		switch (self.dir) {
		case 'U':
			if (self.yPos <= 0) self.yPos = yMax - 1;
			else self.yPos--;
			break;
		case 'D':
			if (self.yPos >= yMax - 1) self.yPos = 0;
			else self.yPos++;
			break;
		case 'L':
			if (self.xPos <= 0) self.xPos = xMax - 1;
			else self.xPos--;
			break;
		case 'R':
			if (self.xPos >= xMax - 1) self.xPos = 0;
			else self.xPos++;
			break;
		}

		//simply snake- if the o touches the ., it will eat the =.
		if (self.yPos == item1->y && self.xPos == item1->x) {
			randomPos(item1);
			addLength(&self);
		}
	}
	return 0;
}


