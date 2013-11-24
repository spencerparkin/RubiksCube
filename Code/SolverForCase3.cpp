// SolverForCase3.cpp

#include "Header.h"

//==================================================================================================
SolverForCase3::SolverForCase3( void )
{
}

//==================================================================================================
/*virtual*/ SolverForCase3::~SolverForCase3( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCase3::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	if( rubiksCube->SubCubeMatrixSize() != 3 )
		return false;

	return false;
}

// SolverForCase3.cpp