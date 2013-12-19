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
	wxScopedPtr< RubiksCube > rubiksCubeCopy;
	rubiksCubeCopy.reset( new RubiksCube( rubiksCube->SubCubeMatrixSize(), false ) );
	if( !rubiksCubeCopy->Copy( *rubiksCube, RubiksCube::CopyMap ) )
		return false;

	RubiksCube::RotationSequence subRotationSequence;

	int maxSequenceLength = 70 * rubiksCube->SubCubeMatrixSize();
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

	return true;
}

//==================================================================================================
Solver::SituationStack::SituationStack( const RubiksCube* rubiksCube )
{
	this->rubiksCube = rubiksCube;
}

//==================================================================================================
Solver::SituationStack::~SituationStack( void )
{
	while( situationList.size() > 0 )
		Pop();
}

//==================================================================================================
const Solver::SituationStack::Situation* Solver::SituationStack::Top( void )
{
	if( situationList.size() == 0 )
		return 0;

	return &situationList.back();
}

//==================================================================================================
void Solver::SituationStack::Push( const RubiksCube::Rotation& rotation )
{
	RubiksCube::RotationSequence rotationSequence;
	rotationSequence.push_back( rotation );
	Push( rotationSequence );
}

//==================================================================================================
void Solver::SituationStack::Push( const RubiksCube::RotationSequence& rotationSequence )
{
	Situation situation;

	const RubiksCube* copyCube = rubiksCube;
	const Situation* top = Top();
	if( top )
		copyCube = top->rubiksCube;

	situation.rotationSequence = rotationSequence;
	situation.rubiksCube = new RubiksCube( copyCube->SubCubeMatrixSize(), false );
	situation.rubiksCube->Copy( *copyCube, RubiksCube::CopyMap );
	situation.rubiksCube->ApplySequence( rotationSequence );

	situationList.push_back( situation );
}

//==================================================================================================
void Solver::SituationStack::Pop( void )
{
	if( situationList.size() == 0 )
		return;

	Situation* top = &situationList.back();
	delete top->rubiksCube;
	situationList.pop_back();
}

//==================================================================================================
void Solver::SituationStack::AppendRotationSequence( RubiksCube::RotationSequence& rotationSequence ) const
{
	for( SituationList::const_iterator iter = situationList.begin(); iter != situationList.end(); iter++ )
	{
		const Situation* situation = &*iter;
		rotationSequence.insert( rotationSequence.end(), situation->rotationSequence.begin(), situation->rotationSequence.end() );
	}
}

// Solver.cpp