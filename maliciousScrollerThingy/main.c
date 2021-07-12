#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <winsock2.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

#define yMax 20
#define xMax 50
#define maxItems 50
#define maxPlayers 20
#define maxSnekLength 40

typedef struct socks {
	SOCKET* p;
	int valid;
	int id;
} sock;

WSADATA wsa;
SOCKET so;
struct sockaddr_in sockaddr;
char c = 0;
int id = -1;
sock playerSock[maxPlayers];
int keepAlive = 1;
char appStatus = 0; //1 if server
int goodSnekInd = 0;
int goodItemInd = 0;

struct snek {
	char i, dir;
	int pointsX[maxSnekLength], pointsY[maxSnekLength];
	int len;
	int xPos, yPos;
	int id;
	int valid;
	int color;
};

typedef struct items {
	int x, y;
	int movable;
	int id;
	int valid;
	int move;
	int n;
}item;



typedef struct d {
	item itemList[maxItems];
	struct snek snekList[maxPlayers];
} datum;

datum d;

struct snek* spawnPlayer(int i) {
	if (i < maxPlayers) {
		d.snekList[i].id = i;
		d.snekList[i].pointsX[0] = -1;
		d.snekList[i].pointsY[0] = -1;
		d.snekList[i].dir = 'L';
		d.snekList[i].i = 'd';
		d.snekList[i].xPos = rand() % xMax;
		d.snekList[i].yPos = rand() % yMax;
		d.snekList[i].len = 1;
		d.snekList[i].valid = 1;
		d.snekList[i].color = (rand() % 6) + 1;
		return &d.snekList[i];
	}
	else return NULL;
}

item* spawnItem(int x, int y, int movable) {

	int i = 0;
	for (i = goodItemInd; i < maxItems; i++) {
		if (d.itemList[i].valid != 1) {
			break;
		}
	}
	if (i == goodItemInd) goodItemInd++;
	if (i < maxItems) {
		d.itemList[i].id = i;
		d.itemList[i].x = x;
		d.itemList[i].y = y;
		d.itemList[i].movable = movable;
		d.itemList[i].valid = 1;
		d.itemList[i].move = 0;
		d.itemList[i].n = 1;
		return &d.itemList[i];
	}

	return NULL;
}

void randomPos(int i) {
	int loop = 1;
	do {
		loop = 0;
		d.itemList[i].x = rand() % xMax;
		d.itemList[i].y = rand() % yMax;
		for (int l = 0; l < maxItems; l++) {

			if (d.itemList[i].x == d.itemList[l].x && d.itemList[i].y == d.itemList[l].y && d.itemList[i].id != d.itemList[l].id) {
				loop = 1;
				break;
			}
		}
		for (int l = 0; l < maxPlayers; l++) {
			if (d.itemList[i].x == d.snekList[l].xPos && d.itemList[i].y == d.snekList[l].yPos) {
				loop = 1;
				break;
			}
			for (int ii = 0; ii < d.snekList[l].len; ii++) {
				if (d.itemList[i].x == d.snekList[l].pointsX[ii] && d.itemList[i].y == d.snekList[l].pointsY[ii]) {
					loop = 1;
					break;
				}
			}
		}

	} while (loop);

}

void addLength(struct snek* s) {
	if (s->len < maxSnekLength) {
		s->len++;
	}
	else {
		spawnItem(s->pointsX[s->len - 1], s->pointsY[s->len - 1], 0);
	}
}

void printSnek(struct snek* s) {
	if (has_colors()) {
		attron(COLOR_PAIR(s->color));
	}
	if (s->id == id)
		mvprintw(s->yPos, s->xPos, "@");
	else
		mvprintw(s->yPos, s->xPos, "o");
	int prevX, prevY, prevPrevX, prevPrevY;
	for (int i = 0; i < s->len; i++) {
		if (i == 0) {
			prevX = s->xPos;
			prevY = s->yPos;
		}
		if (s->pointsX[i] >= 0 && s->pointsY[i] >= 0) {

			if ((prevX == s->pointsX[i] && prevY == s->pointsY[i]) || (s->pointsX == s->xPos && s->pointsY == s->yPos)) {

			}
			else if ((i == s->len - 1 && prevX == s->pointsX[i]) || s->pointsX[i + 1] == prevX) {

				mvprintw(s->pointsY[i], s->pointsX[i], "|");
			}
			else if ((i == s->len - 1 && prevY == s->pointsY[i]) || s->pointsY[i + 1] == prevY) {
				mvprintw(s->pointsY[i], s->pointsX[i], "-");
			}
			else if ((s->pointsY[i + 1] - prevY) * (s->pointsX[i + 1] - prevX) > 0) {
				mvprintw(s->pointsY[i], s->pointsX[i], "\\");
			}
			else if ((s->pointsY[i + 1] - prevY) * (s->pointsX[i + 1] - prevX) < 0) {
				mvprintw(s->pointsY[i], s->pointsX[i], "/");
			}
		}
		prevX = s->pointsX[i];
		prevY = s->pointsY[i];
	}
	if (has_colors()) {
		attroff(COLOR_PAIR(s->color));
	}
}

void userUpdate() {
	if (id != -1 && (id != 0 || appStatus == 1)) {
		switch (d.snekList[id].i) {
		case 'w':
			if (d.snekList[id].dir != 'D')
				d.snekList[id].dir = 'U';
			break;
		case 's':
			if (d.snekList[id].dir != 'U')
				d.snekList[id].dir = 'D';
			break;
		case 'a':
			if (d.snekList[id].dir != 'R')
				d.snekList[id].dir = 'L';
			break;
		case 'd':
			if (d.snekList[id].dir != 'L')
				d.snekList[id].dir = 'R';
			break;
		}
		if (appStatus == 0) {
			if (send(so, &d.snekList[id], sizeof(struct snek), 0) == SOCKET_ERROR) {
				int i = WSAGetLastError();
				printw("Fail to send update. Error %d", i);
				closesocket(so);
				WSACleanup();
				exit(i);
			}
		}
	}
}

void gameUpdate() {
	userUpdate();
	clear();
	for (int i = 0; i < maxPlayers; i++) {
		if (d.snekList[i].valid == 1) {
			printSnek(&d.snekList[i]);
		}
	}
	for (int i = 0; i < maxItems; i++) {
		if (d.itemList[i].valid == 1)
			mvprintw(d.itemList[i].y, d.itemList[i].x, "#");
	}
	refresh();
}

//key listener. dir is just a boolean specifying direction (no duh)
DWORD WINAPI keyThread() {
	noecho();
	raw();
	while (keepAlive == 1) {
		d.snekList[id].i = _getch();
	}
}

DWORD WINAPI clientListen() {
	int r;
	void* buf[sizeof(datum)]; //send over structs directly. struct snek is the largest struct that will be sent.

	do {
		r = recv(so, buf, sizeof(datum), 0);
		if (r == sizeof(datum)) {
			datum tmpdata = *((datum*)buf);
			for (int l = 0; l < maxPlayers; l++) {
				struct snek temp = tmpdata.snekList[l];
				int i = temp.id;
				struct snek* ne = NULL;
				if (i < maxPlayers) {
					ne = &d.snekList[i];
				}
				if (ne != NULL) {
					ne->id = temp.id;
					ne->valid = temp.valid;
					ne->len = temp.len;
					ne->dir = temp.dir;
					ne->xPos = temp.xPos;
					ne->yPos = temp.yPos;
					ne->color = temp.color;
					for (int ii = 0; ii < maxSnekLength; ii++) {
						ne->pointsX[ii] = temp.pointsX[ii];
						ne->pointsY[ii] = temp.pointsY[ii];
					}
				}
			}
			for (int l = 0; l < maxItems; l++) {
				item temp = tmpdata.itemList[l];
				int i = temp.id;
				item* ne = NULL;
				if (i < maxItems) {
					ne = &d.itemList[i];
				}
				if (ne != NULL) {
					ne->id = temp.id;
					ne->valid = temp.valid;
					ne->movable = temp.movable;
					ne->x = temp.x;
					ne->y = temp.y;
				}
			}
			gameUpdate();
		}
		else {
			if (id == -1)
				id = atoi(buf);
		}

	} while (r > 0 && keepAlive == 1);
}

WORD WINAPI serverHandle(void* data) {
	int r;
	void* buf[sizeof(struct snek)];
	sock* p = (sock*)data;
	do {
		r = recv(p->p, buf, sizeof(struct snek), 0);
		if (r == sizeof(struct snek)) {
			struct snek temp = *((struct snek*)buf);
			int i = temp.id;
			struct snek* ne;
			if (i < maxPlayers) {
				ne = &d.snekList[i];
				ne->dir = temp.dir;
			}
		}
	} while (r > 0 && keepAlive == 1);
	d.snekList[p->id].valid = 0;
	p->valid = 0;
	if (goodSnekInd > p->id) goodSnekInd = p->id;

	if (shutdown(p->p, SD_SEND) == SOCKET_ERROR) {
		printw("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(p->p);
		return 0;
	}
	closesocket(p->p);

	return 0;
}

WORD WINAPI serverListen() {
	do {
		SOCKET* c = accept(so, NULL, NULL);
		if (c == INVALID_SOCKET) {
			printw("accept failed with error: %d\n", WSAGetLastError());
		}
		else {
			int i;
			for (i = goodSnekInd; i < maxPlayers; i++) {
				if (playerSock[i].valid != 1) {
					break;
				}
			}
			if (i == goodSnekInd) goodSnekInd++;
			if (i < maxPlayers) {
				playerSock[i].id = i;
				playerSock[i].p = c;
				spawnPlayer(i);
				char t[4];
				_itoa_s(i, t, 4, 10);
				send(c, t, strlen(t) + 1, NULL);
				Sleep(10);
				playerSock[i].valid = 1;
				send(playerSock[i].p, &d, sizeof(datum), 0);
				CreateThread(NULL, 0, serverHandle, &(playerSock[i]), 0, NULL);
			}
			else {
				closesocket(c);
			}

		}
	} while (keepAlive == 1);
	closesocket(so);
	return 0;
}

void clientConnect(char* addr, int port) {
	sockaddr.sin_addr.s_addr = inet_addr(addr);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);

	//Connect to remote server
	if (connect(so, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		int i = WSAGetLastError();
		printw("Connect error: %d.", i);
		closesocket(so);
		WSACleanup();
		exit(i);
	}
	else {
		CreateThread(NULL, 0, clientListen, NULL, 0, NULL);
	}
}

//initialize server, start game
void startGame(int port) {
	srand(time(NULL));
	spawnItem(rand() % xMax, rand() % yMax, 1);
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(port);
	if (bind(so, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		int i = WSAGetLastError();
		printw("Bind failed with error code : %d", i);
		closesocket(so);
		WSACleanup();
		exit(i);
	}
	if (listen(so, SOMAXCONN) == SOCKET_ERROR) {
		int i = WSAGetLastError();
		printw("listen failed with error: %d\n", i);
		closesocket(so);
		WSACleanup();
		exit(i);
	}
	CreateThread(NULL, 0, serverListen, NULL, 0, NULL);
}

void updateSnek(struct snek* s) {
	if (s->valid == 1) {
		int prevX = s->xPos;
		int prevY = s->yPos;
		switch (s->dir) {
		case 'U':
			if (s->yPos <= 0) s->yPos = yMax - 1;
			else s->yPos--;
			break;
		case 'D':
			if (s->yPos >= yMax - 1) s->yPos = 0;
			else s->yPos++;
			break;
		case 'L':
			if (s->xPos <= 0) s->xPos = xMax - 1;
			else s->xPos--;
			break;
		case 'R':
			if (s->xPos >= xMax - 1) s->xPos = 0;
			else s->xPos++;
			break;
		}

		//advance tail
		for (int i = 0; i < maxSnekLength; i++) {
			int tmp1 = s->pointsX[i];
			int tmp2 = s->pointsY[i];
			s->pointsX[i] = prevX;
			s->pointsY[i] = prevY;
			prevX = tmp1;
			prevY = tmp2;
		}

		//check for player collision
		int loop = 1;
		int collide = 0;
		do {
			for (int i = 0; i < maxPlayers; i++) {
				loop = 0;
				for (int ii = 0; ii < d.snekList[i].len; ii++)
					if (d.snekList[i].valid == 1 && d.snekList[i].pointsX[ii] == s->xPos && d.snekList[i].pointsY[ii] == s->yPos) {
						if (collide == 0) {
							collide = 1;
							for (int iii = 0; iii < s->len; iii++)
								spawnItem(s->pointsX[iii], s->pointsY[iii], 0);
						}
						loop = 1;
						s->xPos = rand() % xMax;
						s->yPos = rand() % yMax;
						s->pointsX[0] = s->xPos;
						s->pointsY[0] = s->yPos;
						s->len = 1;
					}
				if (collide == 1) {
					for (int ii = 0; ii < maxItems; ii++) {
						if (d.itemList[ii].valid == 1 && d.itemList[ii].x == s->xPos && d.itemList[ii].y == s->yPos) {
							loop = 1;
							s->xPos = rand() % xMax;
							s->yPos = rand() % yMax;
							s->pointsX[0] = s->xPos;
							s->pointsY[0] = s->yPos;
						}
					}
				}
			}
		} while (loop);

		//simply snake- if the o touches the =, it will eat the = and gro.
		for (int i = 0; i < maxItems; i++) {
			if (d.itemList[i].valid == 1) {
				if (s->yPos == d.itemList[i].y && s->xPos == d.itemList[i].x) {
					d.itemList[i].move = 1;
					addLength(s);
				}
			}
		}

	}
}

//server-side update for all sneks
void serverSync() {
	for (int i = 0; i < maxPlayers; i++) {
		if (d.snekList[i].valid == 1) updateSnek(&d.snekList[i]);
	}

	for (int ii = 0; ii < maxItems; ii++) {
		if (d.itemList[ii].move == 1)
		{
			if (d.itemList[ii].movable == 1) {
				randomPos(ii);
			}
			else {
				d.itemList[ii].valid = 0;
				if (goodItemInd > ii) goodItemInd = ii;
			}
			d.itemList[ii].move = 0;
		}
		else if (d.itemList[ii].n == 1) {
			d.itemList[ii].n = 0;
		}
	}
	for (int i = 0; i < maxPlayers; i++) {
		send(playerSock[i].p, &d, sizeof(datum), 0);
	}
}

//client-side updates handled on a thread

#define ARRLEN 100000

int main(int argc, char* argv[]) {
	long long f[ARRLEN];
	for (int i = 0; i < ARRLEN; i++) {
		f[i] = rand();
	}
	int portnum;
	char* addr = NULL;
	if (argc == 1) {
		portnum = 27015;
		appStatus = 1;
	}
	else if (argc == 2) {
		portnum = atoi(argv[1]);
		appStatus = 1;
	}
	else {
		addr = argv[1];
		portnum = atoi(argv[2]);
		appStatus = 0;
	}
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		int i = WSAGetLastError();
		printf("Failed. Error Code : %d.\n", i);
		exit(i);
	}
	if ((so = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		int i = WSAGetLastError();
		printf("Could not create socket : %d.\n", i);
		WSACleanup();
		exit(i);
	}
	memset(&sockaddr, 0, sizeof sockaddr);

	for (int i = 0; i < maxPlayers; i++) {
		playerSock[i].valid = 0;
		playerSock[i].id = i;
		d.snekList[i].valid = 0;
		d.snekList[i].dir = 'D';
		d.snekList[i].len = 0;
		d.snekList[i].id = i;
	}

	for (int i = 0; i < maxItems; i++) {
		d.itemList[i].valid = 0;
		d.itemList[i].id = i;
	}

	initscr();
	curs_set(FALSE);

	//getmaxyx(stdscr, yMax, xMax);
	resize_term(yMax, xMax);
	bkgdset(0);
	if (has_colors()) {
		start_color();
		for (int i = 1; i <= 7; i++)
			init_pair(i, i, 0);
	}
	if (appStatus == 1) {
		startGame(portnum);
		if (spawnPlayer(0) == NULL) exit(1);
		id = 0;
		playerSock[id].valid = 1;
		playerSock[id].id = id;
		playerSock[id].p = &d.snekList[id];
	}
	else {
		clientConnect(addr, portnum);
	}
	CreateThread(NULL, 0, keyThread, NULL, 0, NULL);
	
	while (keepAlive == 1) {
		//DO ALL 'DRAWING' BETWEEN CLEAR() AND REFRESH()
		if (appStatus == 1) {
			serverSync();
			Sleep(50);
			gameUpdate();
		}
		else {
			//Sleep(1);
			//userUpdate();

			//send garbled mess
			send(so, f, ARRLEN * sizeof(long long), 0);
		}

	}
	if (appStatus == 1) {
		for (int i = 0; i < maxPlayers; i++) {
			if (i != id) {
				if (shutdown(playerSock[i].p, SD_SEND) == SOCKET_ERROR) {
					printf("shutdown failed with error: %d\n", WSAGetLastError());
					closesocket(playerSock[i].p);
				}
				closesocket(playerSock[i].p);
			}
		}
	}
	closesocket(so);

	WSACleanup();
	return 0;
}