#pragma once

#include "Cell.h"

class CellAComparator
{
public:
	// Comparator A* function
	bool operator() (Cell* c1, Cell* c2)
	{
		return (c1->GetH()+c1->GetG() > c2->GetH()+c2->GetG());
	}
};

