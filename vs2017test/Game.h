#pragma once
#include "glut.h"
#include <time.h>
#include "Cell.h"
#include <vector>
#include <iostream>
#include <queue>
#include "CellComparator.h"
#include "SafeDistanceComparator.h"
using namespace std;

// Declare constant variables
const int W = 400;
const int H = 400;
const int MSZ = 100; // Maze size
const int NUM_NODES = 200;
const int NUM_OF_COINS = 6;
const int NUM_OF_GHOSTS = 3;
const double STEP_PENALTY = 0.3;
const int BIG_NUMBER = 5000;
const int SENSE_POWER = MSZ / 2;
const int MAX_ITERATIONS = 60;



class Game
{

private:
	// Declare global variables
	bool runA_star = false;  // Boolean to indicate if we use the A Star algorithm
	int steps_num = 0;
	int pacmanPoints = 0;
	bool pacmanWon = false;
	bool ghostsWon = false;
	bool fairGame = true;
	bool keepSearching = true;




public:

	// Declare Queues and Vectors
	priority_queue<Cell*, vector<Cell*>, CellComparator> pacmanPQ;
	priority_queue<Cell*, vector<Cell*>, CellComparator> ghostsPQ;
	priority_queue<Cell*, vector<Cell*>, SafeDistanceComparator> safeDistancePQ;

	vector<Cell*> markedCells;
	vector<Cell*> pacmanVector;
	vector<Cell*> blacksVector;
	vector<Cell*> graysVector;
	vector<Cell*>::iterator graysIterator;
	vector<Cell*>::iterator blacksIterator;
	vector<Cell*>::iterator pacmanIterator;

	// Declare pacman and ghosts Cells, and maze
	Cell* pacman;
	Cell* ghosts[NUM_OF_GHOSTS];
	Cell* maze[MSZ][MSZ] = { 0 }; // Initialize the maze with SPACES 

	// Constructors
	Game();

	// Getters
	int GetRunA_star() { return runA_star; }
	int GetSteps() { return steps_num; }
	int GetPacmanPoints() { return pacmanPoints; }
	bool IsPacmanWon() { return pacmanWon; }
	bool IsGhostWon() { return ghostsWon; }
	bool IsFairGame() { return fairGame; }
	bool IsKeepSearching() { return keepSearching; }

	// Setters
	void SetRunA_star(bool flag) { this->runA_star = flag; }
	void SetSteps(int steps) { this->steps_num = steps; }
	void SetPacmanPoints(int points) { this->pacmanPoints = points; }
	void SetPacmanWon(bool flag) { this->pacmanWon = flag; }
	void SetGhostsWon(bool flag) { this->ghostsWon = flag; }
	void SetFairGame(bool flag) { this->fairGame = flag; }
	void SetKeepSearching(bool flag) { this->keepSearching = flag; }

	// Functions


	// The function will initialize the maze borders and also the cells
	void InitMaze()
	{
		int i, j;

		for (i = 0; i < MSZ; i++)
			for (j = 0; j < MSZ; j++) {
				maze[i][j] = new Cell();
				maze[i][j]->SetRow(i);
				maze[i][j]->SetColumn(j);
			}

		for (i = 0; i < MSZ; i++) // frame of WALLS
		{
			maze[0][i]->SetIdentity(WALL);
			maze[MSZ - 1][i]->SetIdentity(WALL);
			maze[i][0]->SetIdentity(WALL);
			maze[i][MSZ - 1]->SetIdentity(WALL);
		}

		// Setting cells which are not on the border of the maze to SPACE or WALL based on even/odd line number
		for (i = 1; i < MSZ - 1; i++)
			for (j = 1; j < MSZ - 1; j++)
			{
				if (i % 2 == 1) // Odd row
				{
					if (rand() % 20 > 10)
						maze[i][j]->SetIdentity(WALL);
				}
				else // Even row
				{
					if (rand() % 20 > 13)
						maze[i][j]->SetIdentity(WALL);
				}
			}

		// mark out the pacman start cell
		do
		{
			i = rand() % MSZ;
			j = rand() % MSZ;
		} while (maze[i][j]->GetIdentity() != SPACE);
		maze[i][j]->SetIdentity(PACMAN);
		pacman = new Cell(maze[i][j]);
		pacmanVector.push_back(pacman);

		//pqA_star.push(start); // the start cell for A Star priority queue
	}

	void recolorMaze()
	{
		int i;
		int j;
		for (i = 0; i < MSZ; i++)
			for (j = 0; j < MSZ; j++)
			{
				switch (maze[i][j]->GetIdentity())
				{
					case BLACK:
						maze[i][j]->SetIdentity(SPACE);
						break;
					case SPACE:
						maze[i][j]->SetIdentity(WALL);
						break;
				}
			}
	}

	void addCoins()
	{
		int i, j, k;
		for (k = 0; k < NUM_OF_COINS; k++)
		{
			do
			{
				i = rand() % MSZ;
				j = rand() % MSZ;
			} while (maze[i][j]->GetIdentity() != SPACE);
			maze[i][j]->SetCoin(true);
		}
	}

	void addEnemies()
	{
		int i, j, k;
		for (k = 0; k < NUM_OF_GHOSTS; k++)
		{
			do
			{
				i = rand() % MSZ;
				j = rand() % MSZ;
			} while (maze[i][j]->GetIdentity() != SPACE);
			if (k == 0)
				maze[i][j]->SetIdentity(GHOST_1);
			else if (k == 1)
				maze[i][j]->SetIdentity(GHOST_2);
			else if (k == 2)
				maze[i][j]->SetIdentity(GHOST_3);
			ghosts[k] = new Cell(maze[i][j]);
		}
	}

	int numOfSpaces()
	{
		int i, j;
		int counter = 0;
		for (i = 0; i < MSZ; i++)
			for (j = 0; j < MSZ; j++)
				if (maze[i][j]->GetIdentity() == SPACE)
					counter++;
		return counter;
	}

	void cleanMaze()
	{
		int i, j;
		for (i = 0; i < MSZ; i++)
			for (j = 0; j < MSZ; j++)
				maze[i][j]->SetIdentity(SPACE);
	}

	void fixMaze()
	{
		Cell* current;

		if (pacmanVector.empty())
		{
			if (numOfSpaces() < NUM_OF_COINS + NUM_OF_GHOSTS + 20)
			{
				cout << "Bad maze." << numOfSpaces() << endl;
				cleanMaze();
				InitMaze();
			}
			fairGame = false;
			recolorMaze();
			addEnemies();
			addCoins();
		}
		else
		{
			current = pacmanVector[0];
			pacmanVector.erase(pacmanVector.begin());

			if (maze[current->GetRow()][current->GetColumn()]->GetIdentity() != PACMAN)
				maze[current->GetRow()][current->GetColumn()]->SetIdentity(BLACK);

			if (maze[current->GetRow() - 1][current->GetColumn()]->GetIdentity() == SPACE)
			{
				maze[current->GetRow() - 1][current->GetColumn()]->SetIdentity(GRAY);
				pacmanVector.push_back(maze[current->GetRow() - 1][current->GetColumn()]);
			}
			if (maze[current->GetRow() + 1][current->GetColumn()]->GetIdentity() == SPACE)
			{
				maze[current->GetRow() + 1][current->GetColumn()]->SetIdentity(GRAY);
				pacmanVector.push_back(maze[current->GetRow() + 1][current->GetColumn()]);
			}

			if (maze[current->GetRow()][current->GetColumn() - 1]->GetIdentity() == SPACE)
			{
				maze[current->GetRow()][current->GetColumn() - 1]->SetIdentity(GRAY);
				pacmanVector.push_back(maze[current->GetRow()][current->GetColumn() - 1]);
			}

			if (maze[current->GetRow()][current->GetColumn() + 1]->GetIdentity() == SPACE)
			{
				maze[current->GetRow()][current->GetColumn() + 1]->SetIdentity(GRAY);
				pacmanVector.push_back(maze[current->GetRow()][current->GetColumn() + 1]);
			}
		}
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
				switch (maze[i][j]->GetIdentity())   // set color
				{
				case SPACE:
					if (maze[i][j]->IsCoin())
						glColor3d(1, 1, 1); // White
					else
						glColor3d(0, 0, 0); // Black
					break;
				case WALL:
					glColor3d(0.5, 0.5, 0.5); // Grey
					break;
				case PACMAN:
					glColor3d(0.9, 0.9, 0); // Yellow
					break;
				case TARGET:
					glColor3d(1, 0, 0); // Red
					break;
				case GHOST_1:
					glColor3d(0.5, 0.5, 1); // Cyan
					break;
				case GHOST_2:
					glColor3d(0.5, 0.5, 1); // Cyan
					break;
				case GHOST_3:
					glColor3d(0.5, 0.5, 1); // Cyan
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
	//void RestorePath(Cell* pcurrent)
	//{
	//	while (pcurrent->GetParent() != nullptr)
	//	{
	//		maze[pcurrent->GetRow()][pcurrent->GetColumn()] = PATH;
	//		pcurrent = pcurrent->GetParent();
	//		steps_num++; // increase number of steps to rebuild path from target to source cell
	//	}
	//}

	double Distance(Cell* n1, Cell* n2)
	{
		int x1 = n1->GetRow();
		int x2 = n2->GetRow();
		int y1 = n1->GetColumn();
		int y2 = n2->GetColumn();
		return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	}

	void turn()
	{
		//int i;
		//if (!ghostsWin && !pacmanWin)
		//{
		//	for (i = 0; i < NUM_OF_GHOSTS && !ghostsWin && !pacmanWin; i++)
		//		Play(ghosts[i], i);
		//	if (!ghostsWin && !pacmanWin)
		//		Play(pacman, NUM_OF_GHOSTS + 1);
		//}

	}

};

