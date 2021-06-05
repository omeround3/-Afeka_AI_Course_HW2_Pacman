#include "Game.h"

bool fairGame = true;

// Game Constructor
Game::Game()
{
	InitMaze();
		
	while (this->IsFairGame())
		fixMaze();
}





