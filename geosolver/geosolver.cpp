// geosolver.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "geosolver.h"


// This is an example of an exported variable
GEOSOLVER_API int ngeosolver=0;

// This is an example of an exported function.
GEOSOLVER_API int fngeosolver(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see geosolver.h for the class definition
Cgeosolver::Cgeosolver()
{
	return;
}
