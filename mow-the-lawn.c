#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SECOND 1000000

int score = 0;
int gas = 50;
int gasCounter = 0;
int damage = 0;
int width = 40;
int height = 20;
int xpos, ypos, direction;
int flower_xpos, flower_ypos;
int rock_xpos, rock_ypos;

int isMowed[40][20];			// stores flags of whether tile is mowed or not
int isFlower[40][20];			// stores flags of whether tile has flower or not
int isRock[40][20];		// stores flags of whether tile has rock or not

int isWall[40][20];		// stores wall flags to check for wall

void raw_mode_on() {
    system("stty raw -echo");
}

void raw_mode_off() {
    system("stty -raw echo");
}

void render() {
	int x, y;

	system("clear");

	printf("\033[92mMOW THE LAWN		v0.5\033[39m\r\n\n");

	printf("\033[93mControls: w=UP / a=LEFT / s=DOWN / d=RIGHT / x=EXIT\033[39m\r\n\n");

	printf("\033[93mGas: %d        Damage: %d\033[39m\r\n", gas, damage);

	printf("\r\n\033[93mScore: %d\033[39m\r\n", score);

	// pick random number for

	// print
	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			// check if we need to print a wall
			if((y == 0 || y == (height - 1)) || (x == 0 || x == (width - 1))) {
				printf("\033[96m&\033[39m");
			} else if(x == xpos && y == ypos) {
				// print player if needed
				if(direction == 0) printf("\033[96m<\033[39m");
				if(direction == 1) printf("\033[96m>\033[39m");
				if(direction == 2) printf("\033[96m^\033[39m");
				if(direction == 3) printf("\033[96mv\033[39m");
				isMowed[x][y] = 1;									// set mowed flag on player's grass
			} else if(isFlower[x][y] == 1) {
				if(isMowed[x][y] == 1) {
					printf(" ");
				} else {
					printf("\033[93m*\033[39m");
					flower_xpos = x;
					flower_ypos = y;
				}
			} else if(isRock[x][y] == 1) {
				if(isMowed[x][y] == 1) {
					printf(" ");
				} else {
					printf("\033[90m@\033[39m");
					rock_xpos = x;
					rock_ypos = y;
				}
			} else {
				// Otherwise check if mowed and print grass
				if(isMowed[x][y] == 1) {				// grass is mowed, don't print the grass
					printf(" ");
				} else {
					printf("\033[92m#\033[39m");		// otherwise do
				}
			}

		}
		printf("\r\n");
	}
}

int main() {
	raw_mode_on();

	char c;

	xpos = (width - 2) / 2;
	ypos = 1;
	direction = 1;

	srand(time(0)); // set up random number generator

	int i, j;

	// populate array of mowed flags
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			isMowed[i][j] = 0;				// nothing is mowed yet
		}
	}

	// decide flower positions
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			int fnum = (rand() % (30 - 1 + 1)) + 1;

			if(fnum == 1) {
				if(!((i == 0 || i == 39) || (j == 0 || j == 19))) {
					isFlower[i][j] = 1;
				}
			} else {
				isFlower[i][j] = 0;
			}
		}
	}

	// decide rock positions
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			int rnum = (rand() % (50 - 1 + 1)) + 1;

			if(rnum == 1) {
				if(isFlower[i][j] != 1) {
					isRock[i][j] = 1;
				} else {
					isRock[i][j] = 0;
				}
			} else {
				isRock[i][j] = 0;
			}
		}
	}

	// set wall positions
	for(j = 0; j < height; j++) {
		for(i = 0; i < width; i++) {
			if((j == 0 || j == (height - 1)) || (i == 0 || i == (width - 1))) {
				isWall[i][j] = 1;
				isMowed[i][j] = 1;
			} else {
				isWall[i][j] = 0;
			}
		}
	}

	// game loop
	while(1) {
		render();

		if(damage == 3) {
			printf("\033[91mYou hit too many rocks and broke your mower!\033[39m\r\n\n");
			raw_mode_off();
			exit(0);
		}

		if(gas <= 0) {
			printf("\033[91mYou ran out of gas! Game over!\033[39m\r\n\n");
			raw_mode_off();
			exit(0);
		}

		c = getchar();

		int tmpX = xpos;		// save current x and y to reset if needed
		int tmpY = ypos;

		if(c == 'x') {
			break;
		} else if(c == 'd') {
			direction = 1;
			xpos++;
		} else if(c == 'a') {
			direction = 0;
			xpos--;
		} else if(c == 's') {
			direction = 3;
			ypos++;
		} else if(c == 'w') {
			direction = 2;
			ypos--;
		}

		if(isMowed[xpos][ypos] == 0) {
			score++;
		}

		if(isFlower[xpos][ypos] == 1) {
			score -= 5;
			isFlower[xpos][ypos] = 0;
			printf("\033[91mMOM SAYS: DON'T RUN OVER MY FLOWERS!\033[39m\r\n");
			usleep(SECOND);
		}

		if(isRock[xpos][ypos] == 1) {
			damage++;
			isRock[xpos][ypos] = 0;
			printf("\033[91mYou've damaged your mower! Be careful!\033[39m\r\n");
			usleep(SECOND);
		}

		if(isWall[xpos][ypos] == 1) {
			xpos = tmpX;
			ypos = tmpY;
		}

		gasCounter++;

		if(gasCounter == 5) {
			gas--;
			gasCounter = 0;
		}

	}


	raw_mode_off();

}
