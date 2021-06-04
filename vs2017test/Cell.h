#pragma once


// Declare constant variables
const int SPACE = 0; // Reperesents a blank space where we can "travel" in the maze
const int WALL = 1; // Represents the Walls of the Maze
const int PACMAN = 2; // Represents the START point
const int TARGET = 3; // Represents the TARGET point
const int BLACK = 4; // visited
const int GRAY = 5; // Neighbours to visit
const int PATH = 6; // The path found from PACMAN to TARGET
const int COIN = 7;
const int GHOST_1 = 8;
const int GHOST_2 = 9;
const int GHOST_3 = 10;

class Cell
{
	private:
		int row, column;
		int h; // heuristic
		int g; // distance from source
		int f; // f = g + h
		int t_r;	// target row
		int t_c;	// target column
		Cell* parent;
		bool isCoin;
		int identity;
		double safeDistance;

	public:
		// Constructors
		Cell();
		Cell(Cell* other);
		Cell(int r, int c, Cell* p, int t_r, int t_c);
		~Cell();

		// Getters
		int GetH() { return h; }
		int GetG() { return g; }
		int GetRow() { return row; }
		int GetColumn() { return column; }
		Cell* GetParent() { return parent; }
		bool IsCoin() { return isCoin; }
		int GetIdentity() { return identity; }
		double GetSafeDistance() { return safeDistance; }
		int GetF() { return f; }

		// Setters
		void SetH(int h) { this->h = h; SetF(); }
		void SetG(int g) { this->g = g; SetF(); }
		void SetParent(Cell* parent) { this->parent = parent; }
		void SetRow(int r) { this->row = r; }
		void SetColumn(int c) { this->column = c; }
		int CalculateH();
		int CalculateG();
		void SetCoin(bool flag) { this->isCoin = flag; }
		void SetIdentity(int identity) { this->identity = identity; }
		void SetSafeDistance(double safeDistance) { this->safeDistance = safeDistance; }
		void SetF() { this->f = GetG() + GetH(); }
};

