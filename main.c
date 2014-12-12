
#include "main.h"
#include "GL/glut.h"
#include "time.h"
#include "stdlib.h"
#include "stdio.h"

double status_glut_y = 0.2;
double status_screen_y;

int grid_x = 10;
int grid_y = 10;
double grid_scale_x;
double grid_scale_y;

int screen_x = 640;
int screen_y = 480;
double screen_scale_x;
double screen_scale_y;

double glut_scale = 2.0;

int num_mines = 10;
int * mine_grid;
int * disp_grid;

int num_flags;

int playing = 0;

int start_time;
int current_time;
int elapsed_time;

char * time_str;
char * flags_str;

int main(int argc, char ** argv)
{
	time_str = (char*)malloc(3);
	flags_str = (char*)malloc(2);

	mine_grid = (int*)malloc(grid_x*grid_y*sizeof(int));
	disp_grid = (int*)malloc(grid_x*grid_y*sizeof(int));

	status_screen_y = (screen_y/glut_scale)*status_glut_y;
	grid_scale_x = glut_scale/grid_x;
	grid_scale_y = (glut_scale-status_glut_y)/grid_y;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(screen_x, screen_y);
	glutCreateWindow("Minesweeper");

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutIdleFunc(idleFunc);
	glutMouseFunc(mouseFunc);
	glutKeyboardFunc(keyboardFunc);
	//glutKeyboardUpFunc(keyboardUpFunc);

	startGame();

	glutMainLoop();

	return 0;
}

void displayFunc(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawStatus();

	int x, y;
	for (y = 0; y < grid_y; y++)
	{
		for (x = 0; x < grid_x; x++)
		{
			drawTile(x, y);
		}
	}

	glutSwapBuffers();
}

void reshapeFunc(int x, int y)
{
	glViewport(0, 0, x, y);
	screen_scale_x = x/grid_x;
	screen_scale_y = (y-status_screen_y)/grid_y;
}

void idleFunc(void)
{
	if (playing)
	{
		current_time = glutGet(GLUT_ELAPSED_TIME);
		elapsed_time = current_time-start_time;
	}
	glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y)
{
	if (playing == 1 && state == 0)
	{
		int x_ = x/screen_scale_x;
		int y_ = grid_y - ((y-status_screen_y)/screen_scale_y);

		switch(button)
		{
			case GLUT_LEFT_BUTTON:
				// remove tile
				leftClick(x_, y_);
				break;
			case GLUT_RIGHT_BUTTON:
				// toggle tile flag
				rightClick(x_, y_);
				break;
		}
	}
}

void keyboardFunc(unsigned char key, int x, int y)
{
	printf("%#04x\r\n", key);
	switch (key)
	{
		case 0x1B:
			exit(0);
			break;
        case 0x0D:
            if (playing == 0)
            {
                startGame();
            }
	}
}

void drawStatus(void)
{
	// colour status bar
	glBegin(GL_POLYGON);
	glColor3f(0.8, 0.8, 0.8);
	glVertex3f(-1, 1 - status_glut_y, 0.0);
	glVertex3f(1, 1 - status_glut_y, 0.0);
	glVertex3f(1, 1, 0.0);
	glVertex3f(-1, 1, 0.0);
	glEnd();

	// draw left box
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(-0.99, 0.81, 0.0);
	glVertex3f(-0.81, 0.81, 0.0);
	glVertex3f(-0.81, 0.99, 0.0);
	glVertex3f(-0.99, 0.99, 0.0);
	glEnd();

	// draw time
	sprintf(time_str, "%d", elapsed_time/1000);
	glColor3f(0.8, 0.0, 0.0);
	glRasterPos2f(-0.9, 0.9);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, time_str);

	// draw right box
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.0, 0.0);
	glVertex3f(0.99, 0.81, 0.0);
	glVertex3f(0.81, 0.81, 0.0);
	glVertex3f(0.81, 0.99, 0.0);
	glVertex3f(0.99, 0.99, 0.0);
	glEnd();

	// draw flag count
	sprintf(flags_str, "%d", num_mines - num_flags);
	glColor3f(0.8, 0.0, 0.0);
	glRasterPos2f(0.9, 0.9);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, flags_str);
}

void drawTile(int x, int y)
{
	double x_ = (x*grid_scale_x)-1;
	double y_ = (y*grid_scale_y)-1;
	double xx = grid_scale_x/20;
	double yy = grid_scale_y/20;

	char c = ' ';

	// draw tile
	glBegin(GL_POLYGON);
	switch (disp_grid[x + (y * grid_y)])
	{
		case UNFLAGGED:
			glColor3f(0.8, 0.8, 0.8);
			break;
		case FLAGGED:
			glColor3f(0.8, 0.0, 0.0);
			break;
		case EXPLODED:
			glColor3f(1.0, 1.0, 0.0);
			break;
		case INCORRECT:
			glColor3f(1.0, 0.0, 1.0);
			break;
		case CORRECT:
			glColor3f(0.0, 0.8, 0.0);
			break;
		default:
			glColor3f(0.5, 0.5, 0.5);
			c = disp_grid[x + (y*grid_y)] + 48;
			break;
	}
	glVertex3f(x_, y_, 0.0);
	glVertex3f(x_ + grid_scale_x, y_, 0.0);
	glVertex3f(x_ + grid_scale_x, y_ + grid_scale_y, 0.0);
	glVertex3f(x_, y_ + grid_scale_y, 0.0);
	glEnd();
	
	// draw dark lines
	glBegin(GL_LINES);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(x_+xx, y_+yy, 0.0);
	glVertex3f(x_-xx + grid_scale_x, y_+yy, 0.0);
	glVertex3f(x_-xx + grid_scale_x, y_+yy, 0.0);
	glVertex3f(x_-xx + grid_scale_x, y_-yy + grid_scale_y, 0.0);
	glEnd();
	
	// draw light lines
	glBegin(GL_LINES);
	glColor3f(0.5, 0.5, 0.5);
	glVertex3f(x_-xx + grid_scale_x, y_-yy + grid_scale_y, 0.0);
	glVertex3f(x_+xx, y_-yy + grid_scale_y, 0.0);
	glVertex3f(x_+xx, y_-yy + grid_scale_y, 0.0);
	glVertex3f(x_+xx, y_+yy, 0.0);
	glEnd();

	// draw mine count
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2f(x_ + grid_scale_x/2, y_ + grid_scale_y/2);
	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void leftClick(int x, int y)
{
	if (mine_grid[x + (y*grid_y)] == MINE)
	{
		// explode
		disp_grid[x + (y*grid_y)] = EXPLODED;
		endGame();
	}
	else
	{
		// nothing
		disp_grid[x + (y*grid_y)] = countMines(x, y);
		if (disp_grid[x + (y*grid_y)] == 0)
		{
			revealMines(x, y);
		}
	}
}

void rightClick(int x, int y)
{
	switch (disp_grid[x + (y*grid_y)])
	{
		case FLAGGED:
			num_flags++;
			disp_grid[x + (y*grid_y)] = UNFLAGGED;
			break;
		case UNFLAGGED:
			if (num_flags > 0)
			{
				num_flags--;
				disp_grid[x + (y*grid_y)] = FLAGGED;
				if (getMatches() == num_mines && getUnflagged() == 0)
				{
					endGame();
				}
			}
			break;
	}
}

void revealMines(int x, int y)
{
	int y_,x_;
	for (y_ = y - 1; y_ < y + 2; y_++)
	{
		for (x_ = x - 1; x_ < x + 2; x_++)
		{
			if (y_ > -1 && y_ < grid_y && x_ > -1 && x_ < grid_x)
			{
				if ((y_ != y || x_ != x) && disp_grid[x_ + (y_*grid_y)] < 0)
				{
					leftClick(x_, y_);
				}
			}
		}
	}
}

int countMines(int x, int y)
{
	int y_,x_;
	int mines = 0;
	for (y_ = y - 1; y_ < y + 2; y_++)
	{
		for (x_ = x - 1; x_ < x + 2; x_++)
		{
			if (y_ > -1 && y_ < grid_y && x_ > -1 && x_ < grid_x)
			{
				if (mine_grid[x_ + (y_*grid_y)] == MINE)
				{
					mines++;
				}
			}
		}
	}
	return mines;
}

int getMatches(void)
{
	int g;
	int matches = 0;
	for (g = 0; g < grid_x * grid_y; g++)
	{
		if (disp_grid[g] == FLAGGED && mine_grid[g] == MINE)
		{
			matches++;
		}
	}
	return matches;
}

int getUnflagged(void)
{
	int g;
	int unflagged = 0;
	for (g = 0; g < grid_x * grid_y; g++)
	{
		if (disp_grid[g] == UNFLAGGED)
		{
			unflagged++;
		}
	}
	return unflagged;
}

void startGame(void)
{
	num_flags = num_mines;

	int g;
	for (g = 0; g < grid_x*grid_y; g++)
	{
		mine_grid[g] = SAFE;
		disp_grid[g] = UNFLAGGED; 
	}

	// generate random mines
	srand(time(NULL));
	int m = 0;
	int x,y;
	do
	{
		x = rand() % grid_x;
		y = rand() % grid_y;
		if (mine_grid[x + (y * grid_y)] != MINE)
		{
			mine_grid[x + (y * grid_y)] = MINE;
			m++;
		}
	} while(m < num_mines);

	playing = 1;
	start_time = glutGet(GLUT_ELAPSED_TIME);
}

void endGame(void)
{
	playing = 0;
	int g;
	for (g = 0; g < grid_x * grid_y; g++)
	{
		if (disp_grid[g] == FLAGGED)
		{
			if (mine_grid[g] == MINE)
			{
				disp_grid[g] = CORRECT;
			}
			else
			{
				disp_grid[g] = INCORRECT;
			}
		}
	}
}
