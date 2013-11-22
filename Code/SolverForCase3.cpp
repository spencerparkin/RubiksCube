// SolverForCase3.cpp

#include "Header.h"

//==================================================================================================
SolverForCase3::SolverForCase3( RubiksCube* rubiksCube ) : Solver( rubiksCube )
{
	wxASSERT( rubiksCube->SubCubeMatrixSize() == 3 );
}

//==================================================================================================
/*virtual*/ SolverForCase3::~SolverForCase3( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCase3::MakeMoveSequence( MoveSequence& moveSequence )
{
	return false;
}

// SolverForCase3.cpp