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
const double DISTANCE_THRESHOLD = 1.3;
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
		srand(time(0));
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
	void AddGhosts()
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
			AddGhosts();
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
	
	// A function to check if the Cell's identity is GHOST
	bool IsGhost(int ghostValue)
	{
		if (ghostValue == GHOST_1 || ghostValue == GHOST_2 || ghostValue == GHOST_3)
			return true;
		return false;
	}

	// A function to check the neighbor cells
	void CheckNeighbors(int horizontalOffset, int verticalOffset, Cell* temp, int ghostValue, int ghostNumber)
	{
		Cell* otherTemp;

		// Get the Cell with the correspoding offset
		otherTemp = maze[temp->GetRow() + horizontalOffset][temp->GetColumn() + verticalOffset];
		// Find that Cell in the blacks Vector
		blacksIterator = find(blacksVector.begin(), blacksVector.end(), otherTemp);
		// Find that Cell in the grays Vector
		graysIterator = find(graysVector.begin(), graysVector.end(), otherTemp);
		// Check if the Cell is not a WALL, and if it was the last element on both blacks and grays Vector
		// Also, verify the Cell is not a Ghost
		if (otherTemp->GetIdentity() != WALL && graysIterator == graysVector.end() && blacksIterator == blacksVector.end() && !IsGhost(otherTemp->GetIdentity()))
		{
			// Set the Neighboring Cell's parent to the Ghost
			otherTemp->SetParent(temp);
			// Push the Neighboring Cell to the grays Vector
			graysVector.push_back(otherTemp);
			// Set the Neighboring Cell G to the Ghost's G + some step penalty
			otherTemp->SetG(temp->GetG() + STEP_PENALTY);
			if (IsGhost(ghostValue))
			{
				// Set the Neighboring Cell H to the Distance between the Ghost
				// and the Neighboring Cell
				otherTemp->SetH(Distance(ghosts[ghostNumber], otherTemp));
				ghostsPQ.push(otherTemp); // Push Neighbor to ghosts Priorty Queue
			}
			else if (ghostValue == PACMAN)
			{
				// if the value is Pacman, set the H to the Distance between pacman and the neighbor
				otherTemp->SetH(Distance(pacman, otherTemp));
				pacmanPQ.push(otherTemp);
			}
		}
	}

	// This functions moves the Pacman character to the next cell which is closer to some Food
	void MovePacman(Cell* target)
	{
		Cell* temp = nullptr;
		// Push pacman Cell to the pacman Priorty Queue
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

			CheckNeighbors(0, -1, temp, PACMAN, 0);
			CheckNeighbors(0, 1, temp, PACMAN, 0);
			CheckNeighbors(-1, 0, temp, PACMAN, 0);
			CheckNeighbors(1, 0, temp, PACMAN, 0);
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


	// This functions moves the Ghost character to the next cell which is closer to Pacman
	void MoveGhost(int ghostNumber, int ghostValue)
	{
		Cell* temp = nullptr;

		// Pop a ghost from the ghosts Priorty Queue and check its neighbors
		while (!ghostsPQ.empty())
		{
			temp = ghostsPQ.top();
			ghostsPQ.pop();

			if (temp->GetIdentity() == PACMAN)
				break;
			// Return an iterator to the first element in the range given that compares equal
			// to the temp Cell. If no such element is found, the function returns the last element.
			graysIterator = find(graysVector.begin(), graysVector.end(), temp);
			// If the element returned is not the last element, erase is from the vector
			if (graysIterator != graysVector.end())
				graysVector.erase(graysIterator);
			// Push the temp Cell to the blacks Vector
			blacksVector.push_back(temp);

			CheckNeighbors(0, -1, temp, ghostValue, ghostNumber);
			CheckNeighbors(0, 1, temp, ghostValue, ghostNumber);
			CheckNeighbors(-1, 0, temp, ghostValue, ghostNumber);
			CheckNeighbors(1, 0, temp, ghostValue, ghostNumber);
		}

		while (true)
		{
			temp = temp->GetParent();
			if (temp->GetParent()->GetIdentity() == ghostValue)
				break;
		}
		temp->SetIdentity(ghostValue);

		ghosts[ghostNumber] = temp;
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

	// This functions check the Distance between the coin/food cell and all the Ghosts
	// And calculates the risk to move to this coin
	// The distance between Pacman and the coin is also considered in the calculation.
	// 
	double assertSafety(Cell* coinCell)
	{
		int i;
		double risk = 0;
		// Sum the distances between the coins and the ghosts
		for (i = 0; i < NUM_OF_GHOSTS; i++)
		{
			risk += Distance(coinCell, ghosts[i]);
		}
		// If the Distance between the coin and Pacman is smaller then DISTANCE_THRESHOLD
		// Else, reduce the distance between pacman and the coin, multiplied by 3 for safety
		if (Distance(coinCell, pacman) < DISTANCE_THRESHOLD)
			risk = -DBL_MAX;
		else
			risk -= 3 * Distance(coinCell, pacman);
		return risk;
	}

	// This function calculates the distances to each coin/food in the game from ghosts and pacman
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
					// Calculate the safe distance for the coin Cell
					safeDistance = assertSafety(maze[i][j]);
					// Set the coin Cell's Safe Distance
					maze[i][j]->SetSafeDistance(safeDistance);
					safeDistancePQ.push(maze[i][j]);
				}
			}
		}

		// If no coins are found, then Pacman ate them all 
		if (!foundCoin)
		{
			pacmanWon = true;
			cout << "PACMAN WON THE GAME! Yey!" << endl;
		}
	}

	// This function plays a Ghost character; The steps are 
	// 1. Calculate the distance between the Ghost and Pacman; If it is 1, then Ghosts win
	// 2. Else, the Ghost is moved to the next cell which is closer to Pacman
	void PlayGhost(Cell* character, int ghostNumber, int ghostValue)
	{
		// Calculate the distance between a Ghost to Pacman
		double distance = Distance(character, pacman);
		// If the distance equals 1, the Ghosts won the game
		if (distance == 1)
		{
			ghostsWon = true;
			cout << "The Ghosts won the game! Pacman is dead." << endl;
			return;
		}
		// Else, set the Ghost H value and push it to the ghosts Priorty Queue
		// Then, move the ghost to the next cell and clear the ghosts Priorty Queue
		else 
		{
			character->SetH(distance);
			ghostsPQ.push(character);
			MoveGhost(ghostNumber, ghostValue);
			while (!ghostsPQ.empty())
				ghostsPQ.pop();
		}
	}

	// This function plays a Pacman character; The steps are 
	// 1. Calculate the distance between the Ghost and Pacman; If it is 1, then Ghosts win
	// 2. Else, the Ghost is moved to the next cell which is closer to Pacman
	void PlayPacman(Cell* character)
	{
		Cell* target, * oldTarget = nullptr;
		pacmanVector.push_back(pacman);
		CoinsValue(); // Calculates coin distances
		// After the CoinsValue calculatio, there is a chance that no more coins exist
		// In this case, Pacman won and the flag is changed.
		if (pacmanWon)
			return;
		// If pacman hasn't won yet, get a target coin from the safeDistance Priority Queue
		// with the lowest risk (the lowest risk coin will be returned by top()
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

