#include "Cell.h"
#include <stdlib.h>

// Constructor with H and G
Cell::Cell(int r, int c, Cell* p, int t_r, int t_c)
{
	this->row = r;
	this->column = c;
	this->parent = p;
	this->t_r = t_r;
	this->t_c = t_c;
	this->h = CalculateH(); // Calculate H based on target row and column
	this->g = CalculateG(); // Calculate G based on parent row and column

}

int Cell::CalculateH()
{
	if (parent != nullptr)
		return abs(row - t_r) + abs(column - t_c);
	else
		return 0;
}

int Cell::CalculateG()
{
	if (parent != nullptr)
		return abs(row - parent->GetRow()) + abs(column - parent->GetColumn());
	else
		return 0;
}
