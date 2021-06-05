/*
	A program to demonstrate Bidirectional BFS search from START point to TARGET point.
	The program will start a BFS algorithm from the START point and from the TARGET point and the same time.
	The output is the shortest path from the START point to the TARGET point.

	The program uses the OpenGL library; make sure to run as x86 platform.

	Based on AI Course basic BFS code.
	
	@Authors
	Tal Hagag - 313160921
	Omer Lev-Ron - 204573067
*/
#include "glut.h"
#include <time.h>
#include <vector>
#include <iostream>
#include <queue>
#include "Game.h"


using namespace std;

Game* game;

void init()
{
	srand(time(0));
	glClearColor(1, 1, 1, 0);// color of window background
	glOrtho(-1, 1, -1, 1, 0, 1);
}


// Display function implenmenation of the OpenGL
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	game->DrawMaze();
	glutSwapBuffers(); // show all
}

// Idle function implementation of the OpenGL; loop of glut
void idle()
{
	glutPostRedisplay(); // go to display
}

// Options menu to choose the algorithm to run 
void menu(int choice)
{
	switch (choice)
	{
	case 0: // Continue one step in the game
		if (!game->IsGhostWon() && !game->IsPacmanWon()) {
			game->turn();
			display();
		}
		break;
	case 1: // Continue the game until a win 
		while (!game->IsGhostWon() && !game->IsPacmanWon())
		{ 
			game->turn();
			display();
		}
		break;
	case 2: // Restart the Game
		srand(time(0));
		game = new Game();
		display();
		break;
	}
}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Pacman AI Maze");

	glutDisplayFunc(display); // display is window refresh function
	glutIdleFunc(idle);  // idle is background function

	// Initiliaze a game instance
	game = new Game();

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Continue - One Step", 0);
	glutAddMenuEntry("Run Till End of Game", 1);
	glutAddMenuEntry("Restart Game", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	
	init();

	glutMainLoop();
}