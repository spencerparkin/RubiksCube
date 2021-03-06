// Solver.cpp

#include "Header.h"

//==================================================================================================
Solver::Solver( void )
{
}

//==================================================================================================
/*virtual*/ Solver::~Solver( void )
{
}

//==================================================================================================
bool Solver::MakeEntireSolutionSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	if( !GetReady() )
		return false;

	// I don't plan to support solving a bandaged cube...not sure how to do it.
	rubiksCube->EnforceBandaging( false );

	wxScopedPtr< RubiksCube > rubiksCubeCopy;
	rubiksCubeCopy.reset( new RubiksCube( rubiksCube->SubCubeMatrixSize() ) );
	if( !rubiksCubeCopy->Copy( *rubiksCube, RubiksCube::CopyMap ) )
		return false;

	RubiksCube::RotationSequence subRotationSequence;

	int maxSequenceLength = 99999999; //70 * rubiksCube->SubCubeMatrixSize();
	rotationSequence.clear();
	while( signed( rotationSequence.size() ) <= maxSequenceLength )
	{
		subRotationSequence.clear();
		if( !MakeRotationSequence( rubiksCubeCopy.get(), subRotationSequence ) )
			return false;

		if( subRotationSequence.size() == 0 )
			break;
		else
		{
			RubiksCube::CompressRotationSequence( subRotationSequence );
			rubiksCubeCopy->ApplySequence( subRotationSequence );
			rotationSequence.insert( rotationSequence.end(), subRotationSequence.begin(), subRotationSequence.end() );
		}
	}

	if( signed( rotationSequence.size() ) > maxSequenceLength )
	{
		rotationSequence.clear();
		return false;
	}

	// Now compress across the entire solution sequence.
	RubiksCube::CompressRotationSequence( rotationSequence );

	bool solved = rubiksCubeCopy->IsInSolvedState();
	//wxASSERT( solved );

	return true;
}

// Solver.cpp