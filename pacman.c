#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>

#define LEFT  'A'
#define RIGHT 'D'
#define UP    'W'
#define DOWN  'S'

#define VELOCITY 400
#define DIRECTIONDELAY 100
#define INVENCIBILITYTIMER 10000

typedef struct{
	int x;
	int y;
	int points;
	int invincibility;
	int invincibilityTimer;
	char direction;
	char skin;
} Pacman;

typedef struct{
	int x;
	int y;
} Ghost;

typedef struct{
	char **map;
} Map;

char ** createMap(){
	char model[31][28] = {"============================",
					      "=............==............=",
						  "=.====.=====.==.=====.====.=",
						  "=O====.=====.==.=====.====O=",
						  "=.====.=====.==.=====.====.=",
						  "=..........................=",
						  "=.====.==.========.==.====.=",
					      "=.====.==.========.==.====.=",
						  "=......==....==....==......=",
						  "======.===== == =====.======",
						  "     =.===== == =====.=    =",
						  "     =.==          ==.=    =",
						  "     =.== ======== ==.=    =",
						  "======.== =      = ==.======",
						  "|     .   =      =   .     |",
						  "======.== =      = ==.======",
						  "     =.== ======== ==.=     ",
						  "     =.==          ==.=     ",
						  "     =.== ======== ==.=     ",
						  "======.== ======== ==.======",
						  "=............==............=",
						  "=.====.=====.==.=====.====.=",
						  "=.====.=====.==.=====.====.=",
						  "=O..==................==..O=",
						  "===.==.==.========.==.==.===",
						  "===.==.==.========.==.==.===",
						  "=......==....==....==......=",
						  "=.==========.==.==========.=",
						  "=.==========.==.==========.=",
						  "=..........................=",
						  "============================"};
	
	char **map = (char**)malloc(sizeof(char*)*31);

	int i, j;
	for(i = 0; i <= 31; i++){
		map[i] = malloc(sizeof(char)*29);
		
		for(j = 0; j <= 28; j++){
			map[i][j] = model[i][j];
		}
		map[i][29] = '\n';
	}
	
	return map;
}

int validMove(Map map, int posY, int posX){
	if(map.map[posY][posX] != '='){
		return 1;
	}else {
		return 0;
	}
}

int exists(int x, int y, Ghost *ghosts){
	int i, j;
	for(i = 0; i < 2; i++){
		if(ghosts[i].y == y && ghosts[i].x == x){
			return 1;
		}
	}
	
	return 0;
}

void showMap(Map map, Pacman pacman, Ghost *ghosts){
	system("cls");
	int x, y;
	for(y = 0; y < 31; y++){
		for(x = 0; x < 28; x++){
			if(map.map[y][x] == '='){
				printf("%c ", 219);
			}else if(map.map[y][x] == '|'){
				printf("  ");
			}else {
				
				if(exists(x, y, ghosts)){
					printf("A ");
				}else if(pacman.x == x && pacman.y == y){
					printf("%c ", pacman.skin);
				}else {
					printf("%c ", map.map[y][x]);
				}
			}
		}
		printf("\n");
	}
}

void setDirection(Map map, Pacman *pacman){
	int i;
	for(i = 65; i <= 90; i++){
		if(GetAsyncKeyState(i) == -32767){
			switch(i){
				case UP:
					if(validMove(map, pacman->y-1, pacman->x)){
						pacman->direction = UP;
					}
					break;
				case DOWN:
					if(validMove(map, pacman->y+1, pacman->x)){
						pacman->direction = DOWN;
					}
					break;
				case LEFT:
					if(validMove(map, pacman->y, pacman->x-1)){
						pacman->direction = LEFT;
					}
					break;
				case RIGHT:
					if(validMove(map, pacman->y, pacman->x+1)){
						pacman->direction = RIGHT;
					}	
					break;
			}
		}
	}
}

void eat(Map *map, Pacman *pacman){
	switch(map->map[pacman->y][pacman->x]){
		case '.':
			pacman->points++;
			break;
		case 'O':
			pacman->invincibility = 1;
			pacman->invincibilityTimer = clock();
			pacman->skin = 'C';
			break;
	}
	
	map->map[pacman->y][pacman->x] = ' ';
}

void movePacman(Map map, Pacman *pacman){
	switch(pacman->direction){
		case UP:
			if(validMove(map, pacman->y-1, pacman->x)){
				pacman->y--;
			}
			break;
		case DOWN:
			if(validMove(map, pacman->y+1, pacman->x)){
				pacman->y++;
			}
			break;
		case LEFT:
			if(validMove(map, pacman->y, pacman->x-1)){
				if(map.map[pacman->y][pacman->x-1] == '|'){
					pacman->x = 27;
				}else {
					pacman->x--;
				}
			}
			break;
		case RIGHT:
			if(validMove(map, pacman->y, pacman->x+1)){
				if(map.map[pacman->y][pacman->x+1] == '|'){
					pacman->x = 0;
				}else {
					pacman->x++;
				}
			}	
			break;
	}
	
	eat(&map, pacman);
}

void moveGhost(Map map, Ghost *ghost, Pacman *pacman, int *gameover){
	int shortestDist  = calcDist(ghost->x, ghost->y, pacman->x, pacman->y);
	int GHTposX_short = ghost->x;
	int GHTposY_short = ghost->y;

	if(validMove(map, ghost->y, ghost->x+1) && (calcDist(ghost->x+1, ghost->y, pacman->x, pacman->y)) < shortestDist){
		GHTposX_short++;
	}else if(validMove(map, ghost->y, ghost->x-1) && (calcDist(ghost->x-1, ghost->y, pacman->x, pacman->y)) < shortestDist){
		GHTposX_short--;
	}else if(validMove(map, ghost->y+1, ghost->x) && (calcDist(ghost->x, ghost->y+1, pacman->x, pacman->y)) < shortestDist){
		GHTposY_short++;
	}else if(validMove(map, ghost->y-1, ghost->x) && (calcDist(ghost->x, ghost->y-1, pacman->x, pacman->y)) < shortestDist){
		GHTposY_short--;
	}
	
	if(GHTposX_short == ghost->x && GHTposY_short == ghost->y){
		if(validMove(map, ghost->y, ghost->x+1) && pow((ghost->x+1) - pacman->x, 2) < pow(ghost->x - pacman->x, 2)){
			GHTposX_short++;
		}else if(validMove(map, ghost->y, ghost->x-1) && pow((ghost->x-1) - pacman->x, 2) < pow(ghost->x - pacman->x, 2)){
			GHTposX_short--;
		}else if(validMove(map, ghost->y+1, ghost->x) && pow((ghost->y+1) - pacman->y, 2) < pow(ghost->y - pacman->y, 2)){
			GHTposY_short++;
		}else if(validMove(map, ghost->y-1, ghost->x) && pow((ghost->y-1) - pacman->y, 2) < pow(ghost->y - pacman->y, 2)){
			GHTposY_short--;
		}
	}
	
	ghost->x = GHTposX_short;
	ghost->y = GHTposY_short;
	
	if(ghost->x == pacman->x && ghost->y == pacman->y){
		if(pacman->invincibility){
			pacman->points += 200;
			ghost->x = 13;
			ghost->y = 11;
		}else {
			*gameover = 1;
		}
	}
}

int calcDist(int x, int y, int X, int Y){
	return sqrt(pow(x-X, 2)+pow(y-Y, 2));
}

int main(){
	
	int gameover = 0;

	Pacman pacman;
	pacman.x = 13;
	pacman.y = 11;
	pacman.points = 0;
	pacman.invincibility = 0;
	pacman.direction = RIGHT;
	pacman.skin = 'c';
	
	Ghost blinky;
	blinky.x = 6;
	blinky.y = 20;
	
	Ghost clyde;
	clyde.x = 21;
	clyde.y = 5;
	
	Ghost ghosts[2];
	ghosts[0] = blinky;
	ghosts[1] = clyde;
	
	Map map;
	map.map = createMap();
	
	showMap(map, pacman, ghosts);
	
	clock_t timerDD = clock();
	clock_t timerV = clock();
	
	while(!gameover){
		if(clock() - pacman.invincibilityTimer > INVENCIBILITYTIMER && pacman.invincibility){
			pacman.invincibility = 0;
			pacman.skin = 'c';
		}
		
		if(clock() - timerDD > DIRECTIONDELAY){
			timerDD = clock();
			setDirection(map, &pacman);
		}
		
		if(clock() - timerV > VELOCITY){
			timerV = clock();
			
			movePacman(map, &pacman);
		
			int i;
			for(i = 0; i < 2;i++){
				moveGhost(map, &ghosts[i], &pacman, &gameover);
			}
			
			showMap(map, pacman, ghosts);
		}
	}
	
	printf("\n");
	printf("[GAMEOVER]\n");
	printf("SCORE: %ipts\n", pacman.points);
	
	return 0;
}
