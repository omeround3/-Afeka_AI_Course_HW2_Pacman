#pragma once

#include "Cell.h"

class CellComparator
{
	public:
		// Comparator function
		bool operator() (Cell* c1, Cell* c2)
		{
			return (c1->GetF() > c2->GetF());
		}
};

