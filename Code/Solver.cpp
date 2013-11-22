// Solver.cpp

#include "Header.h"

//==================================================================================================
Solver::Solver( RubiksCube* rubiksCube )
{
	this->rubiksCube = rubiksCube;
}

//==================================================================================================
Solver::~Solver( void )
{
}

//==================================================================================================
bool Solver::MakeMove( RubiksCube::Rotation& rotation )
{
	// An idea that comes to mind is that of semi-invariant move sequences.
	// An invariant move sequence, once performed, leaves the cube invariant.
	// A semi-invariant move sequence, once performed, leaves part of the cube invariant
	// while manipulating other parts of the cube.  Both invariant and semi-invariant
	// move sequences can look fairly similar.  A simple class of invariant move
	// sequences takes any move sequence, applies it, and then applies the inverse of
	// that move sequence.  A class of semi-invariant move sequences would do the same,
	// but before applying the inverse move sequence, it would apply a move that
	// does not prevent the inverse move sequence from taking affect, but also moves
	// sub-cubes in the cube to a desired location.  Discovering all types of desired
	// semi-invariant move sequences will be a challenge, if they even exist.

	// TODO: Check to see if the cube is in the solved state here.
	//       If it is, then we're done.

	if( currentMoveSequence.size() == 0 )
		if( !MakeMoveSequence( currentMoveSequence ) || currentMoveSequence.size() == 0 )
			return false;

	MoveSequence::iterator iter = currentMoveSequence.begin();
	rotation = *iter;
	currentMoveSequence.erase( iter );
	return true;
}

// Solver.cpp