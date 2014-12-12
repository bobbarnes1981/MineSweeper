
#pragma once

enum GRID_CONTENT
{
	SAFE = 0,
	MINE = 1
};

enum DISP_CONTENT
{
	INCORRECT	= -5,
	CORRECT		= -4,
	EXPLODED	= -3,
	FLAGGED		= -2,
	UNFLAGGED	= -1
};

int main(int, char **);
void displayFunc(void);
void reshapeFunc(int, int);
void idleFunc(void);
void mouseFunc(int, int, int, int);
void keyboardFunc(unsigned char, int, int);
void drawStatus(void);
void drawTile(int, int);
void leftClick(int, int);
void rightClick(int, int);
void revealMines(int, int);
int countMines(int, int);
int getMatches(void);
int getUnflagged(void);
void startGame(void);
void endGame(void);

