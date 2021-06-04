#include "Game.h"




// Functions
//double Distance(Cell* n1, Cell* n2);
//void fixMaze();


bool fairGame = true;

// Game Constructor
Game::Game()
{
	InitMaze();
		
	while (this->IsFairGame())
		fixMaze();
}



// This function is pushing all the white neighbors to the queue
// if a neighbor is the TARGET point, the serach is done and it will restore the path
//void CheckNeighbor(Cell* pcurrent, int row, int col)
//{
//	// Check the color of the neighbor cell
//	Cell* nextCell;
//
//	if (maze[row][col]->GetIdentity == TARGET)	// Found target
//	{
//		if (runBestFirstSearch)
//		{
//			cout << "Best First Search: The solution has been found\n";
//			runBestFirstSearch = false;
//		}
//		else
//		{
//			cout << "A Star: The solution has been found\n";
//			runA_star = false;
//		}
//		// Restore the path
//		nextCell = new Cell(row, col, pcurrent, target->GetRow(), target->GetColumn());
//		RestorePath(pcurrent);
//
//	}
//	else if (maze[row][col] == SPACE)	// Found space
//	{
//		nextCell = new Cell(row, col, pcurrent, target->GetRow(), target->GetColumn());
//		maze[row][col] = GRAY;
//		if (runBestFirstSearch)
//			pqBFS.push(nextCell);
//		else
//			pqA_star.push(nextCell);
//	}
//}
//
//
//// RunA Star iteration on the Maze
//void RunA_Star()
//{
//	Cell* pcurrent;
//
//	if (pqA_star.empty())
//	{
//		cout << "There is no solution.\n";
//		runA_star = false;
//	}
//	else // there are gray cells
//	{
//		pcurrent = pqA_star.top();	// save the element with the lowest F=H+G value
//		pqA_star.pop();	// remove it from the priority queue
//		if (maze[pcurrent->GetRow()][pcurrent->GetColumn()] != PACMAN && maze[pcurrent->GetRow()][pcurrent->GetColumn()] != TARGET)
//			maze[pcurrent->GetRow()][pcurrent->GetColumn()] = BLACK;   // and paint it black
//
//		// Check the neighbors
//		// Up
//		if (runA_star)
//			CheckNeighbor(pcurrent, pcurrent->GetRow() + 1, pcurrent->GetColumn());
//		// Down
//		if (runA_star)
//			CheckNeighbor(pcurrent, pcurrent->GetRow() - 1, pcurrent->GetColumn());
//		// Right
//		if (runA_star)
//			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() + 1);
//		// Left
//		if (runA_star)
//			CheckNeighbor(pcurrent, pcurrent->GetRow(), pcurrent->GetColumn() - 1);
//	}
//
//
//}



