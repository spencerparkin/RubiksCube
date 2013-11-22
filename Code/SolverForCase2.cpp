// SolverForCase2.cpp

#include "Header.h"

//==================================================================================================
SolverForCase2::SolverForCase2( RubiksCube* rubiksCube ) : Solver( rubiksCube )
{
	wxASSERT( rubiksCube->SubCubeMatrixSize() == 2 );
}

//==================================================================================================
/*virtual*/ SolverForCase2::~SolverForCase2( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCase2::MakeMoveSequence( MoveSequence& moveSequence )
{
	return false;
}

// SolverForCase3.cpp