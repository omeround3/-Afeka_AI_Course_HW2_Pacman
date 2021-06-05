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
const int W = 1000;
const int H = 1000;
const int MSZ = 50; // Maze size
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

		// Setting cells which are not on the border of the maze to SPACE or WALL based on even/odd row number
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

	// A function to chnage the Maze BLACK Cells to SPACE and SPACE to WALL
	void ChangeColorMaze()
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

	// A function to add the Pacman food (coins) to the maze
	void AddFood()
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

	// This functions add the Monsters to the Maze
	void AddMonsters()
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

	// A function to count the number of spaces in the Maze
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

	// This functions cleans the Maze and sets all the Cells to SPACE
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
			if (numOfSpaces() < NUM_OF_COINS + NUM_OF_GHOSTS + 30)
			{
				cout << "An incorrect maze was created. Drawing another maze." << numOfSpaces() << endl;
				cleanMaze();
				InitMaze();
			}
			fairGame = false;
			ChangeColorMaze();
			AddMonsters();
			AddFood();
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
					glColor3d(1, 1, 0); // Yellow
					break;
				case TARGET:
					glColor3d(1, 0, 0); // Red
					break;
				case GHOST_1:
					glColor3d(0.5, 0.1, 1); // Purple
					break;
				case GHOST_2:
					glColor3d(0.5, 0.1, 1); // Purple
					break;
				case GHOST_3:
					glColor3d(0.5, 0.1, 1); // Purple
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

	
	bool IsGhost(int value)
	{
		if (value == GHOST_1 || value == GHOST_2 || value == GHOST_3)
			return true;
		return false;
	}

	void checkSide(int horizontalOffset, int verticalOffset, Cell* temp, int value, int ghostNumber)
	{
		Cell* otherTemp;

		otherTemp = maze[temp->GetRow() + horizontalOffset][temp->GetColumn() + verticalOffset];
		blacksIterator = find(blacksVector.begin(), blacksVector.end(), otherTemp);
		graysIterator = find(graysVector.begin(), graysVector.end(), otherTemp);
		if (otherTemp->GetIdentity() != WALL && graysIterator == graysVector.end() && blacksIterator == blacksVector.end() && !IsGhost(otherTemp->GetIdentity()))
		{
			otherTemp->SetParent(temp);
			graysVector.push_back(otherTemp);
			otherTemp->SetG(temp->GetG() + STEP_PENALTY);
			if (IsGhost(value))
			{
				otherTemp->SetH(Distance(ghosts[ghostNumber], otherTemp));
				ghostsPQ.push(otherTemp);
			}
			else if (value == PACMAN)
			{
				otherTemp->SetH(Distance(pacman, otherTemp));
				pacmanPQ.push(otherTemp);
			}
		}
	}

	void MovePacman(Cell* target)
	{
		Cell* temp = nullptr;
		pacmanPQ.push(pacman);
		while (!pacmanPQ.empty())
		{
			temp = pacmanPQ.top();
			pacmanPQ.pop();

			if (temp->GetRow() == target->GetRow() && temp->GetColumn() == target->GetColumn())
				break;

			graysIterator = find(graysVector.begin(), graysVector.end(), temp);
			if (graysIterator != graysVector.end())
				graysVector.erase(graysIterator);
			blacksVector.push_back(temp);

			checkSide(0, -1, temp, PACMAN, 0);
			checkSide(0, 1, temp, PACMAN, 0);
			checkSide(-1, 0, temp, PACMAN, 0);
			checkSide(1, 0, temp, PACMAN, 0);
		}

		while (true)
		{
			temp = temp->GetParent();
			if (temp->GetParent()->GetIdentity() == PACMAN)
				break;
		}
		temp->SetIdentity(PACMAN);
		if (temp->IsCoin())
			temp->SetCoin(false);

		pacman = temp;
		temp = temp->GetParent();

		maze[temp->GetRow()][temp->GetColumn()]->SetIdentity(SPACE);

		if (Distance(pacman, target) == 1)
		{
			target->SetCoin(false);
			pacmanPoints += 1;
			cout << "Pacman found food and got " << pacmanPoints * 100 << " points" << endl;
			if (pacmanPoints == NUM_OF_COINS)
				pacmanWon = true;
		}

		blacksVector.clear();
		graysVector.clear();


	}

	void moveGhost(int i, int value)
	{
		Cell* temp = nullptr;

		while (!ghostsPQ.empty())
		{
			temp = ghostsPQ.top();
			ghostsPQ.pop();

			if (temp->GetIdentity() == PACMAN)
				break;

			graysIterator = find(graysVector.begin(), graysVector.end(), temp);
			if (graysIterator != graysVector.end())
				graysVector.erase(graysIterator);
			blacksVector.push_back(temp);

			checkSide(0, -1, temp, value, i);
			checkSide(0, 1, temp, value, i);
			checkSide(-1, 0, temp, value, i);
			checkSide(1, 0, temp, value, i);
		}

		while (true)
		{
			temp = temp->GetParent();
			if (temp->GetParent()->GetIdentity() == value)
				break;
		}
		temp->SetIdentity(value);

		ghosts[i] = temp;
		temp = temp->GetParent();


		maze[temp->GetRow()][temp->GetColumn()]->SetIdentity(SPACE);

		blacksVector.clear();
		graysVector.clear();
	}

	// A function to calculate the Distance between two Cells
	double Distance(Cell* n1, Cell* n2)
	{
		int x1 = n1->GetRow();
		int x2 = n2->GetRow();
		int y1 = n1->GetColumn();
		int y2 = n2->GetColumn();
		return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	}

	double assertSafety(Cell* np)
	{
		int i;
		double risk = 0;
		for (i = 0; i < NUM_OF_GHOSTS; i++)
		{
			risk += Distance(np, ghosts[i]);
		}
		risk -= 3 * Distance(np, pacman);
		if (Distance(np, pacman) < 1.3)
			risk = -DBL_MAX;
		return risk;
	}

	void CoinsValue()
	{
		int i, j;
		double safeDistance;
		bool foundCoin = false;
		for (i = 0; i < MSZ; i++)
		{
			for (j = 0; j < MSZ; j++)
			{
				if (maze[i][j]->IsCoin())
				{
					foundCoin = true;
					safeDistance = assertSafety(maze[i][j]);
					maze[i][j]->SetSafeDistance(safeDistance);
					safeDistancePQ.push(maze[i][j]);
				}
			}
		}

		if (!foundCoin)
		{
			pacmanWon = true;
			cout << "PACMAN WON THE GAME! Yey!" << endl;
		}
	}

	void PlayGhost(Cell* character, int i, int ghostValue)
	{
		double distance = Distance(character, pacman);
		if (distance == 1)
		{
			ghostsWon = true;
			return;
		}
		if (ghostsWon)
			return;
		character->SetH(distance);
		ghostsPQ.push(character);
		moveGhost(i, ghostValue);
		while (!ghostsPQ.empty())
			ghostsPQ.pop();

	}

	void PlayPacman(Cell* character)
	{
		Cell* target, * oldTarget = nullptr;
		pacmanVector.push_back(pacman);
		CoinsValue();
		if (pacmanWon)
			return;
		target = safeDistancePQ.top();
		if (oldTarget != nullptr)
		{
			if (target->GetSafeDistance() < oldTarget->GetSafeDistance())
				MovePacman(oldTarget);
		}
		else
			MovePacman(target);
		oldTarget = target;

		pacmanVector.clear();
		markedCells.clear();
		while (!pacmanPQ.empty())
			pacmanPQ.pop();
		while (!safeDistancePQ.empty())
			safeDistancePQ.pop();

	}

	void Play(Cell* character, int i)
	{
		if (i < NUM_OF_GHOSTS)
		{
			if (i == 0)
				PlayGhost(character, i, GHOST_1);
			if (i == 1)
				PlayGhost(character, i, GHOST_2);
			if (i == 2)
				PlayGhost(character, i, GHOST_3);
		}
		else
			PlayPacman(character);
	}

	void turn()
	{
		int i;
		if (!ghostsWon && !pacmanWon)
		{
			for (i = 0; i < NUM_OF_GHOSTS && !ghostsWon && !pacmanWon; i++)
				Play(ghosts[i], i);
			if (!ghostsWon && !pacmanWon)
				Play(pacman, NUM_OF_GHOSTS + 1);
		}

	}

};

