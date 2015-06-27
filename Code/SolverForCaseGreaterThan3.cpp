// SolverForCaseGreaterThan3.cpp

#include "Header.h"

//==================================================================================================
SolverForCaseGreaterThan3::SolverForCaseGreaterThan3( void )
{
	solverForCase3 = 0;
	reducedCube = 0;
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::~SolverForCaseGreaterThan3( void )
{
	delete solverForCase3;
	delete reducedCube;
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	if( rubiksCube->SubCubeMatrixSize() <= 3 )
		return false;

	// TODO: If the cube is equivilant to the 3x3x3 case, create the reduced cube and its solver here.

	if( reducedCube && solverForCase3 )
	{
		RubiksCube::RotationSequence reducedRotationSequence;
		if( !solverForCase3->MakeRotationSequence( reducedCube, reducedRotationSequence ) || reducedRotationSequence.size() == 0 )
			return false;

		TranslateRotationSequence( rubiksCube, reducedRotationSequence, rotationSequence );

		if( !reducedCube->ApplySequence( reducedRotationSequence ) )
			return false;

		return true;
	}
	else
	{
		// TODO: Get all colors in POS_X(yellow), then POS_Y(blue), POS_Z(red), NEG_X(white), NEG_Y(green), NEG_Z(orange).
		//       Use "TranslateFaceColor".
		//       If yellow found in NEG_X face, move to POS_Y face (arbitrarily).  If yellow found in any of POS_Y, NEG_Y, POS_Z or NEG_Z, move to POS_X face.
		//       Blue can't be in POS_X face.  So if found in NEG_Y face, move to NEG_X face (arbitrarily).  If found in NEG_X, POS_Z or NEG_Z, move to POS_Y face.
		//       Red can't be in POS_X or POS_Y face.  If found in NEG_Z face, move to NEG_X face (arbitrarily).  If in NEG_X or NEG_Y, move to POS_Z face.
		//       Keep going...there is an easy algorithm for moving colors from one face to an adjacent face.

		// TODO: Get the cube into the reduced state here.
	}

	return true;
}

//==================================================================================================
void SolverForCaseGreaterThan3::TranslateRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::RotationSequence& reducedRotationSequence, RubiksCube::RotationSequence& rotationSequence )
{
	for( RubiksCube::RotationSequence::const_iterator iter = reducedRotationSequence.begin(); iter != reducedRotationSequence.end(); iter++ )
	{
		RubiksCube::Rotation reducedRotation = *iter;
		
		switch( reducedRotation.plane.index )
		{
			case 0:
			{
				rotationSequence.push_back( reducedRotation );
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
					rotationSequence.push_back( rotation );
				}

				break;
			}
			case 2:
			{
				RubiksCube::Rotation rotation = reducedRotation;
				rotation.plane.index = rubiksCube->SubCubeMatrixSize() - 1;
				rotationSequence.push_back( rotation );
				break;
			}
		}
	}
}

// SolverForCaseGreaterThan3.cpp