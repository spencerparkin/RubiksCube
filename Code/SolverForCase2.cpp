// SolverForCase2.cpp

#include "Header.h"

//==================================================================================================
SolverForCase2::SolverForCase2( void )
{
}

//==================================================================================================
/*virtual*/ SolverForCase2::~SolverForCase2( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCase2::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	if( rubiksCube->SubCubeMatrixSize() != 2 )
		return false;

	return false;
}

// SolverForCase3.cpp