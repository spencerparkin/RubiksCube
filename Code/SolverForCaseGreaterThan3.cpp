// SolverForCaseGreaterThan3.cpp

#include "Header.h"

//==================================================================================================
SolverForCaseGreaterThan3::SolverForCaseGreaterThan3( RubiksCube* rubiksCube ) : Solver( rubiksCube )
{
	solverForCase3 = 0;
	reducedCube = 0;

	wxASSERT( rubiksCube->SubCubeMatrixSize() > 3 );
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::~SolverForCaseGreaterThan3( void )
{
	delete solverForCase3;
	delete reducedCube;
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::MakeMoveSequence( MoveSequence& moveSequence )
{
	// TODO: If the cube is equivilant to the 3x3x3 case, create the reduced cube and its solver here.

	if( reducedCube && solverForCase3 )
	{
		MoveSequence reducedMoveSequence;
		if( !solverForCase3->MakeMoveSequence( reducedMoveSequence ) || reducedMoveSequence.size() == 0 )
			return false;

		TranslateMoveSequence( reducedMoveSequence, moveSequence );

		while( reducedMoveSequence.size() > 0 )
		{
			MoveSequence::iterator iter = reducedMoveSequence.begin();
			RubiksCube::Rotation rotation = *iter;
			reducedMoveSequence.erase( iter );
			if( !reducedCube->Apply( rotation ) )
				return false;
		}

		return true;
	}
	else
	{
		// TODO: Get the cube into the reduced state here.
	}

	return false;
}

//==================================================================================================
void SolverForCaseGreaterThan3::TranslateMoveSequence( const MoveSequence& reducedMoveSequence, MoveSequence& moveSequence )
{
	for( MoveSequence::const_iterator iter = reducedMoveSequence.begin(); iter != reducedMoveSequence.end(); iter++ )
	{
		RubiksCube::Rotation reducedRotation = *iter;
		
		switch( reducedRotation.plane.index )
		{
			case 0:
			{
				moveSequence.push_back( reducedRotation );
				break;
			}
			case 1:
			{
				// The 3x3x3 solver doesn't ever have to make moves like this, because
				// it needs only rotate the outer planes to solve the puzzle.
				for( int index = 1; index < rubiksCube->SubCubeMatrixSize() - 1; index++ )
				{
					RubiksCube::Rotation rotation = reducedRotation;
					rotation.plane.index = index;
					moveSequence.push_back( rotation );
				}

				break;
			}
			case 2:
			{
				RubiksCube::Rotation rotation = reducedRotation;
				rotation.plane.index = rubiksCube->SubCubeMatrixSize() - 1;
				moveSequence.push_back( rotation );
				break;
			}
		}
	}
}

// SolverForCaseGreaterThan3.cpp