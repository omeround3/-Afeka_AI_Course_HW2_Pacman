/*
	A Class to represent a Game.
	@Authors
	Tal Hagag - 313160921
	Omer Lev-Ron - 204573067
*/

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
const int W = 1000; // Window Width
const int H = 1000; // Windt
const int MSZ = 50; // Maze size
const int NUM_OF_COINS = 4; // Number of coins to eat by Pacman
const int NUM_OF_GHOSTS = 3; // Number of Ghosts
const double STEP_PENALTY = 0.3;
const double DISTANCE_THRESHOLD = 1.3; // Needs to be bigger than 1 and smaller than 2


class Game
{
	private:

	public:
		// Declare global variables
		int steps_num = 0;
		int pacmanPoints = 0;
		bool pacmanWon = false;
		bool ghostsWon = false;
		bool fairGame = true;

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
		int GetSteps() { return steps_num; }
		int GetPacmanPoints() { return pacmanPoints; }
		bool IsPacmanWon() { return pacmanWon; }
		bool IsGhostWon() { return ghostsWon; }
		bool IsFairGame() { return fairGame; }

		// Setters
		void SetSteps(int steps) { this->steps_num = steps; }
		void SetPacmanPoints(int points) { this->pacmanPoints = points; }
		void SetPacmanWon(bool flag) { this->pacmanWon = flag; }
		void SetGhostsWon(bool flag) { this->ghostsWon = flag; }
		void SetFairGame(bool flag) { this->fairGame = flag; }

		// Functions
		void InitMaze();
		void ChangeColorMaze();
		void AddFood();
		void AddGhosts();
		int numOfSpaces();
		void cleanMaze();
		void fixMaze();
		void DrawMaze();
		bool IsGhost(int ghostValue);
		void CheckNeighbors(int horizontalOffset, int verticalOffset, Cell* temp, int ghostValue, int ghostNumber);
		void MovePacman(Cell* target);
		void MoveGhost(int ghostNumber, int ghostValue);
		double Distance(Cell* n1, Cell* n2);
		double assertSafety(Cell* coinCell);
		void CoinsRisk();
		void PlayGhost(Cell* character, int ghostNumber, int ghostValue);
		void PlayPacman();
		void Play(Cell* character, int i);
		void turn();
};

