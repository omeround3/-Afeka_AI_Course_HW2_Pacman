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
#include "Cell.h"
#include <vector>
#include <iostream>
#include <queue>
#include "CellComparator.h"
#include "CellAComparator.h"



using namespace std;

// Declare constant variables
const int W = 1000;
const int H = 1000;
const int MSZ = 100; // Maze size

// Declare global variables
int maze[MSZ][MSZ] = { 0 }; // Initialize the maze with SPACES 
bool runA_star = false;  // Boolean to indicate if we use the A Star algorithm
bool runBestFirstSearch = false; // Boolean to indicate if we use Best Search First algorithm
int steps_num = 0;

priority_queue<Cell*, vector<Cell*>, CellComparator> pqBFS;
priority_queue<Cell*, vector<Cell*>, CellAComparator> pqA_star;

Cell* start;
Cell* target;

//typedef pair<int, Cell>;

void InitMaze();

void init()
{
	srand(time(0));
	glClearColor(1, 1, 1, 0);// color of window background
	glOrtho(-1, 1, -1, 1, 0, 1);
	InitMaze();
}

// The function will initialize the maze borders and also the cells
void InitMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++) // frame of WALLS
	{
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	// Setting cells which are not on the border of the maze to SPACE or WALL based on even/odd line number
	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1) // Odd line
			{
				if (rand() % 100 > 20)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
			else // Even line
			{
				if (rand() % 100 > 35)
					maze[i][j] = WALL;
				else
					maze[i][j] = SPACE;
			}
		}
	// mark out the start and the target cells
	int target_row = rand() % MSZ, target_col = rand() % MSZ;
	maze[MSZ / 2][MSZ / 2] = PACMAN; // Start cell
	maze[target_row][target_col] = TARGET; // Target Cell

	// Create the START and TARGET cells and push to the queues
	start = new Cell(MSZ / 2, MSZ / 2, nullptr, target_row, target_col);
	target = new Cell(target_row, target_col, nullptr, target_row, target_col);

	pqBFS.push(start); // the start cell for Best Search priority queue
	pqA_star.push(start); // the start cell for A Star priority queue
}

// The function will draw the maze according to each cell value in 'maze'
void DrawMaze()
{
	int i, j;
	int xsz = W / MSZ;
	int ysz = H / MSZ;
	double x, y;
	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j])   // set color
			{
			case SPACE:
				glColor3d(1, 1, 1); // White
				break;
			case WALL:
				glColor3d(0.2, 0, 0); // Brown
				break;
			case PACMAN:
				glColor3d(0.5, 0.5, 1); // Cyan
				break;
			case TARGET:
				glColor3d(1, 0, 0); // Red
				break;
			case BLACK:
				glColor3d(0.9, 0.9, 0.5);
				break;
			case GRAY:
				glColor3d(0.2, 0.6, 0.2);
				break;
			case PATH:
				glColor3d(1, 0.6, 1);
				break;
			}
			// Draw square -> maze[i][j]
			x = 2.0 * (j * xsz) / W - 1; // value in range [-1,1)
			y = 2.0 * (i * ysz) / H - 1; // value in range [-1,1)
			// Fill the sqaure 
			glBegin(GL_POLYGON);
			glVertex2d(x, y);
			glVertex2d(x, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y);
			glEnd();
			// Draw the square borders (cleaner look)
			glColor3d(0, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2d(x, y);
			glVertex2d(x, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y + (2.0 * ysz) / H);
			glVertex2d(x + 2.0 * xsz / W, y);
			glEnd();
		}
}

// Searches for a specific cell in the Cells queue and if exists return it
Cell* getCell(int row, int col, vector <Cell*>* grays)
{
	for (Cell* cell : *grays)
	{
		if (cell->GetRow() == row && cell->GetColumn() == col) { return cell; }
	}
	return nullptr;
}

// This function will build and restore the path from START to TARGET
void RestorePath(Cell* pcurrent)
{
	while (pcurrent->GetParent() != nullptr)
	{
		maze[pcurrent->GetRow()][pcurrent->GetColumn()] = PATH;
		pcurrent = pcurrent->GetParent();
		steps_num++; // increase number of steps to rebuild path from target to source cell
	}
}


// This function is pushing all the white neighbors to the queue
// if a neighbor is the TARGET point, the serach is done and it will restore the path
void CheckNeighbor(Cell* pcurrent, int row, int col)
{
	// Check the color of the neighbor cell
	Cell* nextCell;

	if (maze[row][col] == TARGET)	// Found target
	{
		if (runBestFirstSearch)
		{
			cout << "Best First Search: The solution has been found\n";
			runBestFirstSearch = false;
		}
		else
		{
			cout << "A Star: The solution has been found\n";
			runA_star = false;
		}
		// Restore the path
		nextCell = new Cell(row, col, pcurrent, target->GetRow(), target->GetColumn());
		RestorePath(pcurrent);
		
	}
	else if (maze[row][col] == SPACE)	// Found space
	{
		nextCell = new Cell(row, col, pcurrent, target->GetRow(), target->GetColumn());
		maze[row][col] = GRAY;
		if (runBestFirstSearch)
			pqBFS.push(nextCell);
		else
			pqA_star.push(nextCell);
	}
}


// Best First Search iteration on the Maze
void best_first_search()
{
	Cell* pcurrent;

	if (pqBFS.empty())
	{
		cout << "Best First Search: There is no solution.\n";
		runBestFirstSearch = false;
	}
	else // there are gray cells
	{
		pcurrent = pqBFS.top();	// save the element with the lowest H value
		pqBFS.pop();	// remove it from the priority queue
		if (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != PACMAN && maze[pcurrent->GetRow()][pcurrent->GetColumn()] != TARGET)
			maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;   // and paint it black

		// Check the neighbors
		// Up
		if (runBestFirstSearch)
			CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
		// Down
		if (runBestFirstSearch)
			CheckNeighbor(pcurrent, pcurrent->GetRow() - 1, pcurrent->GetColumn());
		// Right
		if (runBestFirstSearch)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
		// Left
		if (runBestFirstSearch)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);
	}
	

}

// RunA Star iteration on the Maze
void RunA_Star()
{
	Cell* pcurrent;

	if (pqA_star.empty())
	{
		cout << "There is no solution.\n";
		runA_star = false;
	}
	else // there are gray cells
	{
		pcurrent = pqA_star.top();	// save the element with the lowest F=H+G value
		pqA_star.pop();	// remove it from the priority queue
		if (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != PACMAN && maze[pcurrent->GetRow()][pcurrent->GetColumn()] != TARGET)
			maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;   // and paint it black

		// Check the neighbors
		// Up
		if (runA_star)
			CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
		// Down
		if (runA_star)
			CheckNeighbor(pcurrent, pcurrent->GetRow() - 1, pcurrent->GetColumn());
		// Right
		if (runA_star)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
		// Left
		if (runA_star)
			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);
	}


}

// Display function implenmenation of the OpenGL
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	DrawMaze();
	glutSwapBuffers(); // show all
}

// Idle function implementation of the OpenGL; loop of glut
void idle()
{
	if (runBestFirstSearch)
		best_first_search();
	if (runA_star)
		RunA_Star();
	glutPostRedisplay(); // go to display
}

// Options menu to choose the algorithm to run 
void menu(int choice)
{
	switch (choice)
	{
	case 0: // Best First Search
		runBestFirstSearch = true;
		break;
	case 1: // A Star
		runA_star = true;
		break;
	}
}


void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(W, H);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("AI Algorithms Maze");

	glutDisplayFunc(display); // display is window refresh function
	glutIdleFunc(idle);  // idle is background function

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Best First Search", 0);
	glutAddMenuEntry("A Star", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();

	glutMainLoop();
}