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
	// An idea that comes to mind is that of semi-preserving move sequences.
	// A preserving move sequence, once performed, leaves the cube invariant.
	// A semi-preserving move sequence, once performed, leaves part of the cube invariant
	// while manipulating other parts of the cube.  Both preserving and semi-preserving
	// move sequences can look fairly similar.  A simple class of preserving move
	// sequences takes any move sequence, applies it, and then applies the inverse of
	// that move sequence.  A class of semi-preserving move sequences would do the same,
	// but before applying the inverse move sequence, it would apply a move that
	// does not entirely prevent the inverse move sequence from taking some affect, and so moves
	// sub-cubes in the cube to a desired location.  Discovering all types of desired
	// semi-preserving move sequences may be a challenge.  One possible means of discovery
	// would be to construct various semi-preserving move sequences, applying them, and
	// then coloring the cube based on only what changed through the entire sequence.
	// Once one has a sufficiently large repitware of move sequences at his disposal,
	// he can figure out how to string those together to complete the cube in stages.
	// For example, if I know that I can complete a face without modifying an adjacent face,
	// given an initial fixed orientation, then I may be able to use that to complete a significant
	// portion of the cube.

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