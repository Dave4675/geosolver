// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GEOSOLVER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GEOSOLVER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#pragma once

#ifdef GEOSOLVER_EXPORTS
#define GEOSOLVER_API __declspec(dllexport)
#else
#define GEOSOLVER_API __declspec(dllimport)
#endif

// This class is exported from the geosolver.dll
class GEOSOLVER_API Cgeosolver {
public:
	Cgeosolver(void);
	// TODO: add your methods here.
};

extern GEOSOLVER_API int ngeosolver;

GEOSOLVER_API int fngeosolver(void);
