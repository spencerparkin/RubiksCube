// SolverForCaseGreaterThan3.cpp

#include "Header.h"

//==================================================================================================
SolverForCaseGreaterThan3::SolverForCaseGreaterThan3( void )
{
	CreateStageSolvers();
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::~SolverForCaseGreaterThan3( void )
{
	DestroyStageSolvers();
}

//==================================================================================================
void SolverForCaseGreaterThan3::CreateStageSolvers( void )
{
	stageSolverList.push_back( new CenterFaceSolver() );

	// Note that we must solve the faces in this order, because the face solver assumes this order in its logic.
	stageSolverList.push_back( new FaceSolver( RubiksCube::POS_X ) );
	stageSolverList.push_back( new FaceSolver( RubiksCube::POS_Y ) );
	stageSolverList.push_back( new FaceSolver( RubiksCube::POS_Z ) );
	stageSolverList.push_back( new FaceSolver( RubiksCube::NEG_X ) );
	stageSolverList.push_back( new FaceSolver( RubiksCube::NEG_Y ) );
	stageSolverList.push_back( new FaceSolver( RubiksCube::NEG_Z ) );		// This should be a no-op.

	stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::BLUE ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::RED ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::GREEN ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::ORANGE ) );

	stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::BLUE ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::RED ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::GREEN ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::ORANGE ) );

	stageSolverList.push_back( new EdgeSolver( RubiksCube::BLUE, RubiksCube::ORANGE ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::ORANGE, RubiksCube::GREEN ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::GREEN, RubiksCube::RED ) );
	stageSolverList.push_back( new EdgeSolver( RubiksCube::RED, RubiksCube::BLUE ) );		// This should be a no-op.

	stageSolverList.push_back( new As3x3x3Solver() );
}

//==================================================================================================
void SolverForCaseGreaterThan3::ResetStageSolvers( void )
{
	StageSolverList::iterator iter = stageSolverList.begin();
	while( iter != stageSolverList.end() )
	{
		StageSolver* stageSolver = *iter;
		stageSolver->SetState( StageSolver::STAGE_PENDING );
		iter++;
	}
}

//==================================================================================================
void SolverForCaseGreaterThan3::DestroyStageSolvers( void )
{
	while( stageSolverList.size() > 0 )
	{
		StageSolverList::iterator iter = stageSolverList.begin();
		StageSolver* stageSolver = *iter;
		stageSolverList.erase( iter );
		delete stageSolver;
	}
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::GetReady( void )
{
	ResetStageSolvers();
	return true;
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	if( rubiksCube->SubCubeMatrixSize() <= 3 )
		return false;

	for( StageSolverList::iterator iter = stageSolverList.begin(); iter != stageSolverList.end(); iter++ )
	{
		StageSolver* stageSolver = *iter;
		if( stageSolver->GetState() == StageSolver::STAGE_PENDING )
		{
			if( stageSolver->SolveStage( rubiksCube, rotationSequence ) )
				return true;
			else if( stageSolver->GetState() != StageSolver::STAGE_COMPLETE )
				return false;
		}
#ifdef SOLVER_DEBUG
		else
		{
			bool solved = stageSolver->VerifyActuallySolved( rubiksCube );
			wxASSERT( solved );
			if( !solved )
				break;
		}
#endif //SOLVER_DEBUG
	}

	return true;
}

//==================================================================================================
// I'm not sure who came up with the edge-parity fix sequence.  It's not obvious or intuitive to me,
// but it was easy to see how it generalizes.
/*static*/ bool SolverForCaseGreaterThan3::GenerateEdgeParityFixSequence( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, const std::vector< int >& planeIndexVector, RubiksCube::RotationSequence& rotationSequence )
{
	if( planeIndexVector.size() == 0 )
		return false;

	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	for( int i = 0; i < ( int )planeIndexVector.size(); i++ )
	{
		int planeIndex = planeIndexVector[i];
		if( planeIndex > subCubeMatrixSize / 2 )
			return false;
	}

	RubiksCube::RelativeRotation relativeRotation;
	relativeRotation.planeIndex = 0;
	RubiksCube::Rotation rotation;

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::D, 2, rotationSequence );

	relativeRotation.type = RubiksCube::RelativeRotation::B;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	//---

	relativeRotation.type = RubiksCube::RelativeRotation::R;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::U, 1, rotationSequence );

	relativeRotation.type = RubiksCube::RelativeRotation::R;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::Di, 1, rotationSequence );

	//---

	relativeRotation.type = RubiksCube::RelativeRotation::R;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::D, 1, rotationSequence );

	relativeRotation.type = RubiksCube::RelativeRotation::R;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	//---

	relativeRotation.type = RubiksCube::RelativeRotation::F;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::D, 1, rotationSequence );

	relativeRotation.type = RubiksCube::RelativeRotation::F;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::Ui, 1, rotationSequence );

	//---

	relativeRotation.type = RubiksCube::RelativeRotation::B;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( rotation );

	BatchRotate( rubiksCube, perspective, planeIndexVector, RubiksCube::RelativeRotation::D, 2, rotationSequence );

	return true;
}

//==================================================================================================
/*static*/ void SolverForCaseGreaterThan3::BatchRotate( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, const std::vector< int >& planeIndexVector, RubiksCube::RelativeRotation::Type type, int rotationCount, RubiksCube::RotationSequence& rotationSequence )
{
	RubiksCube::RelativeRotation relativeRotation;
	relativeRotation.type = type;

	for( int i = 0; i < ( int )planeIndexVector.size(); i++ )
	{
		relativeRotation.planeIndex = planeIndexVector[i];
		RubiksCube::Rotation rotation;
		rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );

		// These should get compressed into one rotation.
		for( int j = 0; j < rotationCount; j++ )
			rotationSequence.push_back( rotation );
	}
}

//==================================================================================================
SolverForCaseGreaterThan3::CenterFaceSolver::CenterFaceSolver( void )
{
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::CenterFaceSolver::~CenterFaceSolver( void )
{
}

//==================================================================================================
// Note that since centers can't move, nothing is really being solved here.
// What we're doing is orienting the cube for the computer to make it easier for the computer.
/*virtual*/ bool SolverForCaseGreaterThan3::CenterFaceSolver::SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	// In this case, there are no center face cubies to solve.
	if( subCubeMatrixSize % 2 == 0 )
	{
		SetState( STAGE_COMPLETE );
		return false;
	}

	int center = ( subCubeMatrixSize - 1 ) / 2;

	const RubiksCube::SubCube* subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, center, center ) );
	if( subCube->faceData[ RubiksCube::POS_X ].color != RubiksCube::YELLOW )
	{
		RubiksCube::Rotation rotation;
		rotation.plane.index = center;
		rotation.angle = 0.0;

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( 0, center, center ) );
		if( subCube->faceData[ RubiksCube::NEG_X ].color == RubiksCube::YELLOW )
		{
			rotation.plane.axis = RubiksCube::Z_AXIS;
			rotation.angle = M_PI;
		}

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, subCubeMatrixSize - 1, center ) );
		if( subCube->faceData[ RubiksCube::POS_Y ].color == RubiksCube::YELLOW )
		{
			rotation.plane.axis = RubiksCube::Z_AXIS;
			rotation.angle = -M_PI / 2.f;
		}

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, 0, center ) );
		if( subCube->faceData[ RubiksCube::NEG_Y ].color == RubiksCube::YELLOW )
		{
			rotation.plane.axis = RubiksCube::Z_AXIS;
			rotation.angle = M_PI / 2.f;
		}

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, center, subCubeMatrixSize - 1 ) );
		if( subCube->faceData[ RubiksCube::POS_Z ].color == RubiksCube::YELLOW )
		{
			rotation.plane.axis = RubiksCube::Y_AXIS;
			rotation.angle = M_PI / 2.f;
		}

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, center, 0 ) );
		if( subCube->faceData[ RubiksCube::NEG_Z ].color == RubiksCube::YELLOW )
		{
			rotation.plane.axis = RubiksCube::Y_AXIS;
			rotation.angle = -M_PI / 2.f;
		}

		wxASSERT( rotation.angle != 0.0 );
		if( rotation.angle == 0.0 )
			return false;

		rotationSequence.push_back( rotation );
		return true;
	}
	
	subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, subCubeMatrixSize - 1, center ) );
	if( subCube->faceData[ RubiksCube::POS_Y ].color != RubiksCube::BLUE )
	{
		RubiksCube::Rotation rotation;
		rotation.plane.index = center;
		rotation.plane.axis = RubiksCube::X_AXIS;
		rotation.angle = 0.0;

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, 0, center ) );
		if( subCube->faceData[ RubiksCube::NEG_Y ].color == RubiksCube::BLUE )
			rotation.angle = M_PI;
		
		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, center, subCubeMatrixSize - 1 ) );
		if( subCube->faceData[ RubiksCube::POS_Z ].color == RubiksCube::BLUE )
			rotation.angle = -M_PI / 2.0;

		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( center, center, 0 ) );
		if( subCube->faceData[ RubiksCube::NEG_Z ].color == RubiksCube::BLUE )
			rotation.angle = M_PI / 2.0;

		wxASSERT( rotation.angle != 0.0 );
		if( rotation.angle == 0.0 )
			return false;

		rotationSequence.push_back( rotation );
		return true;
	}

	// If the yellow and blue center face cubies are where they need to be, then all other center face cubies are positioned correctly.
	SetState( STAGE_COMPLETE );
	return false;
}

//==================================================================================================
SolverForCaseGreaterThan3::StageSolver::StageSolver( void )
{
	SetState( STAGE_PENDING );
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::StageSolver::~StageSolver( void )
{
}

//==================================================================================================
SolverForCaseGreaterThan3::FaceSolver::FaceSolver( RubiksCube::Face face )
{
	this->face = face;
	GeneratePerspectiveList();
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::FaceSolver::~FaceSolver( void )
{
}

//==================================================================================================
bool SolverForCaseGreaterThan3::FaceSolver::VerifyActuallySolved( const RubiksCube* rubiksCube )
{
	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	RubiksCube::Perspective perspective;
	switch( face )
	{
		case RubiksCube::POS_X:
		{
			perspective.rAxis = yAxis;
			perspective.uAxis = zAxis;
			perspective.fAxis = xAxis;
			break;
		}
		case RubiksCube::POS_Y:
		{
			perspective.rAxis = zAxis;
			perspective.uAxis = xAxis;
			perspective.fAxis = yAxis;
			break;
		}
		case RubiksCube::POS_Z:
		{
			perspective.rAxis = xAxis;
			perspective.uAxis = yAxis;
			perspective.fAxis = zAxis;
			break;
		}
		case RubiksCube::NEG_X:
		{
			perspective.rAxis = -zAxis;
			perspective.uAxis = -yAxis;
			perspective.fAxis = -xAxis;
			break;
		}
		case RubiksCube::NEG_Y:
		{
			perspective.rAxis = -xAxis;
			perspective.uAxis = -zAxis;
			perspective.fAxis = -yAxis;
			break;
		}
		case RubiksCube::NEG_Z:
		{
			perspective.rAxis = -yAxis;
			perspective.uAxis = -xAxis;
			perspective.fAxis = -zAxis;
			break;
		}
		default:
		{
			return false;
		}
	}

	RubiksCube::Color color = RubiksCube::TranslateFaceColor( face );
	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	for( int x = 1; x < subCubeMatrixSize - 1; x++ )
	{
		for( int y = 1; y < subCubeMatrixSize - 1; y++ )
		{
			RubiksCube::Coordinates coords;
			coords.x = x;
			coords.y = y;
			coords.z = subCubeMatrixSize - 1;

			const RubiksCube::SubCube* subCube = rubiksCube->Matrix( coords, perspective );
			RubiksCube::Face face = RubiksCube::TranslateNormal( perspective.fAxis );
			if( subCube->faceData[ face ].color != color )
				return false;
		}
	}

	return true;
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::FaceSolver::SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	for( PerspectiveList::iterator iter = perspectiveList.begin(); iter != perspectiveList.end(); iter++ )
	{
		RubiksCube::Perspective perspective = *iter;
		if( FindRotationSequence( rubiksCube, perspective, rotationSequence ) )
			return true;
	}

	SetState( STAGE_COMPLETE );
	return false;
}

//==================================================================================================
bool SolverForCaseGreaterThan3::FaceSolver::FindRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, RubiksCube::RotationSequence& rotationSequence )
{
	RubiksCube::Color color = RubiksCube::TranslateFaceColor( face );

	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	for( int x = 1; x < subCubeMatrixSize - 1; x++ )
	{
		for( int y = 1; y < subCubeMatrixSize - 1; y++ )
		{
			RubiksCube::Coordinates forwardCoords;
			forwardCoords.x = x;
			forwardCoords.y = y;
			forwardCoords.z = subCubeMatrixSize - 1;

			const RubiksCube::SubCube* subCube = rubiksCube->Matrix( forwardCoords, perspective );
			RubiksCube::Face forwardFace = RubiksCube::TranslateNormal( perspective.fAxis );
			if( subCube->faceData[ forwardFace ].color == color )
				return GenerateRotationSequence( rubiksCube, perspective, rotationSequence, forwardCoords );
		}
	}

	return false;
}

//==================================================================================================
bool SolverForCaseGreaterThan3::FaceSolver::GenerateRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, RubiksCube::RotationSequence& rotationSequence, const RubiksCube::Coordinates& forwardCoords )
{
	RubiksCube::Color color = RubiksCube::TranslateFaceColor( face );

	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	RubiksCube::Coordinates upwardCoords;
	upwardCoords.x = forwardCoords.x;
	upwardCoords.y = subCubeMatrixSize - 1;
	upwardCoords.z = subCubeMatrixSize - 1 - forwardCoords.y;

	RubiksCube::Rotation rotation, invRotation;
	RubiksCube::Rotation shiftRotation, invShiftRotation;
	RubiksCube::Rotation preservativeRotation, invPreservativeRotation;

	RubiksCube::RelativeRotation relativeRotation;

	int ccwRotationCount = 0;
	while( ccwRotationCount < 4 )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( upwardCoords, perspective );
		RubiksCube::Face upwardFace = RubiksCube::TranslateNormal( perspective.uAxis );
		if( subCube->faceData[ upwardFace ].color != color )
			break;

		ccwRotationCount++;

		int x = upwardCoords.z;
		int z = subCubeMatrixSize - 1 - upwardCoords.x;

		upwardCoords.x = x;
		upwardCoords.z = z;

		relativeRotation.planeIndex = 0;
		relativeRotation.type = RubiksCube::RelativeRotation::U;
		
		rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
	}

	// This shouldn't be possible!
	if( ccwRotationCount == 4 )
	{
		rotationSequence.clear();
		return false;
	}

	relativeRotation.planeIndex = subCubeMatrixSize - 1 - forwardCoords.x;
	relativeRotation.type = RubiksCube::RelativeRotation::R;
	rubiksCube->TranslateRotation( perspective, relativeRotation, shiftRotation );
	invShiftRotation.SetInverse( shiftRotation );

	relativeRotation.planeIndex = 0;
	bool ccwRotate = true;
	if( forwardCoords.x == subCubeMatrixSize - 1 - forwardCoords.y )
		ccwRotate = false;
	if( ccwRotate )
		relativeRotation.type = RubiksCube::RelativeRotation::Ui;
	else
		relativeRotation.type = RubiksCube::RelativeRotation::U;
	rubiksCube->TranslateRotation( perspective, relativeRotation, rotation );
	invRotation.SetInverse( rotation );

	relativeRotation.type = RubiksCube::RelativeRotation::R;
	if( ccwRotate )
		relativeRotation.planeIndex = forwardCoords.y;
	else
		relativeRotation.planeIndex = subCubeMatrixSize - 1 - forwardCoords.y;
	rubiksCube->TranslateRotation( perspective, relativeRotation, preservativeRotation );
	invPreservativeRotation.SetInverse( preservativeRotation );

	if( shiftRotation.plane.index == preservativeRotation.plane.index )
		return false;

	// Okay, here's the move!
	rotationSequence.push_back( shiftRotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( preservativeRotation );
	rotationSequence.push_back( invRotation );
	rotationSequence.push_back( invShiftRotation );
	rotationSequence.push_back( rotation );
	rotationSequence.push_back( invPreservativeRotation );
	//rotationSequence.push_back( invRotation );		// Shouldn't need this one, but whatever.

	return true;
}

//==================================================================================================
void SolverForCaseGreaterThan3::FaceSolver::GeneratePerspectiveList( void )
{
	RubiksCube::Perspective perspective;

	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	switch( face )
	{
		case RubiksCube::POS_X:			// No faces assumed solved at this point.
		{
			perspective.rAxis = zAxis;
			perspective.uAxis = yAxis;
			perspective.fAxis = -xAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = zAxis;
			perspective.uAxis = xAxis;
			perspective.fAxis = yAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = -yAxis;
			perspective.uAxis = xAxis;
			perspective.fAxis = zAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = -zAxis;
			perspective.uAxis = xAxis;
			perspective.fAxis = -yAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = yAxis;
			perspective.uAxis = xAxis;
			perspective.fAxis = -zAxis;
			perspectiveList.push_back( perspective );

			break;
		}
		case RubiksCube::POS_Y:			// POS_X face assumed solve at this point.
		{
			perspective.rAxis = xAxis;
			perspective.uAxis = zAxis;
			perspective.fAxis = -yAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = xAxis;
			perspective.uAxis = yAxis;
			perspective.fAxis = zAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = -xAxis;
			perspective.uAxis = yAxis;
			perspective.fAxis = -zAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = zAxis;
			perspective.uAxis = yAxis;
			perspective.fAxis = -xAxis;
			perspectiveList.push_back( perspective );

			break;
		}
		case RubiksCube::POS_Z:			// POS_X, POS_Y faces assumed solved at this point.
		{
			perspective.rAxis = xAxis;
			perspective.uAxis = -yAxis;
			perspective.fAxis = -zAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = -yAxis;
			perspective.uAxis = zAxis;
			perspective.fAxis = -xAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = xAxis;
			perspective.uAxis = zAxis;
			perspective.fAxis = -yAxis;
			perspectiveList.push_back( perspective );

			break;
		}
		case RubiksCube::NEG_X:			// POS_X, POS_Y, POS_Z faces assumed solved at this point.
		{
			perspective.rAxis = -yAxis;
			perspective.uAxis = -xAxis;
			perspective.fAxis = -zAxis;
			perspectiveList.push_back( perspective );

			perspective.rAxis = zAxis;
			perspective.uAxis = -xAxis;
			perspective.fAxis = -yAxis;
			perspectiveList.push_back( perspective );

			break;
		}
		case RubiksCube::NEG_Y:			// POS_X, POS_Y, POS_Z, NEG_X faces assumed solved at this point.
		{
			perspective.rAxis = xAxis;
			perspective.uAxis = -yAxis;
			perspective.fAxis = -zAxis;
			perspectiveList.push_back( perspective );

			break;
		}
		case RubiksCube::NEG_Z:			// POS_X, POS_Y, POS_Z, NEG_X, NEG_Y faces assumed solved at this point.
		{
			// The NEG_Z face must then be solved too.  No perspective to add!  :)
			break;
		}
	}
}

//==================================================================================================
SolverForCaseGreaterThan3::EdgeSolver::EdgeSolver( RubiksCube::Color colorA, RubiksCube::Color colorB )
{
	colors[0] = colorA;
	colors[1] = colorB;

	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	RubiksCube::Perspective perspective;

	// Make a list of perspectives for each edge.

	perspective.rAxis = xAxis;
	perspective.uAxis = yAxis;
	perspective.fAxis = zAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = zAxis;
	perspective.uAxis = yAxis;
	perspective.fAxis = -xAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -xAxis;
	perspective.uAxis = yAxis;
	perspective.fAxis = -zAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -zAxis;
	perspective.uAxis = yAxis;
	perspective.fAxis = xAxis;
	perspectiveList.push_back( perspective );

	//---

	perspective.rAxis = xAxis;
	perspective.uAxis = -zAxis;
	perspective.fAxis = yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = yAxis;
	perspective.uAxis = -zAxis;
	perspective.fAxis = -xAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = zAxis;
	perspective.uAxis = xAxis;
	perspective.fAxis = yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = yAxis;
	perspective.uAxis = xAxis;
	perspective.fAxis = -zAxis;
	perspectiveList.push_back( perspective );

	//---

	perspective.rAxis = zAxis;
	perspective.uAxis = -xAxis;
	perspective.fAxis = -yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -yAxis;
	perspective.uAxis = -xAxis;
	perspective.fAxis = -zAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = xAxis;
	perspective.uAxis = zAxis;
	perspective.fAxis = -yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -yAxis;
	perspective.uAxis = zAxis;
	perspective.fAxis = -xAxis;
	perspectiveList.push_back( perspective );
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::EdgeSolver::~EdgeSolver( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::EdgeSolver::SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	const RubiksCube::Perspective* bestPerspective = 0;
	int biggestEdgeCount = 0;
	const RubiksCube::SubCube* subCube = 0;
	RubiksCube::Face rightFace, forwardFace;

	// Find the perspective that views the largest partially or perhaps fully completed version of the edge we want to complete.
	for( PerspectiveList::iterator iter = perspectiveList.begin(); iter != perspectiveList.end(); iter++ )
	{
		const RubiksCube::Perspective& perspective = *iter;

		rightFace = RubiksCube::TranslateNormal( perspective.rAxis );
		forwardFace = RubiksCube::TranslateNormal( perspective.fAxis );

		bool hasCenterEdgeCube = false;
		if( subCubeMatrixSize % 2 == 1 )	// Cubes of even order have no center edge cubies.
		{
			subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, ( subCubeMatrixSize - 1 ) / 2, subCubeMatrixSize - 1 ), perspective );
			if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
				( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
			{
				hasCenterEdgeCube = true;
			}
		}

		int edgeCount = 0;
		for( int y = 1; y < subCubeMatrixSize - 1; y++ )
		{
			RubiksCube::Coordinates coords;
			coords.x = subCubeMatrixSize - 1;
			coords.y = y;
			coords.z = subCubeMatrixSize - 1;

			subCube = rubiksCube->Matrix( coords, perspective );

			if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
				( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
			{
				// This makes odd-order cubes slower to solve in a worst case, because we always choose the perspective with the center edge cube.
				// For cubes of even-order, we can always start with the edge that is most solved.
				if( subCubeMatrixSize % 2 == 0 || hasCenterEdgeCube )
					edgeCount++;
			}
		}

		if( edgeCount > biggestEdgeCount )
		{
			biggestEdgeCount = edgeCount;
			bestPerspective = &perspective;
		}
	}

	if( !bestPerspective )
		return false;

	rightFace = RubiksCube::TranslateNormal( bestPerspective->rAxis );
	forwardFace = RubiksCube::TranslateNormal( bestPerspective->fAxis );

	if( biggestEdgeCount == subCubeMatrixSize - 2 )
	{
		RubiksCube::Color rightColor, forwardColor;
		if( subCubeMatrixSize % 2 == 1 )
		{
			subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, ( subCubeMatrixSize - 1 ) / 2, subCubeMatrixSize - 1 ), *bestPerspective );
			rightColor = subCube->faceData[ rightFace ].color;
			forwardColor = subCube->faceData[ forwardFace ].color;
		}
		else
		{
			// In the case of cubes of even order, determining the proper right and forward face colors is not obvious.
			// For large cubes, we should choose these colors to minimize the number of parity-fix moves we have to do,
			// but to make things simpler here, I'm just going to choose arbitrarily.  Notice that our choice here generates
			// no parity fix in the case that the edge is already solved.
			subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, 1, subCubeMatrixSize - 1 ), *bestPerspective );
			rightColor = subCube->faceData[ rightFace ].color;
			forwardColor = subCube->faceData[ forwardFace ].color;
		}

		std::vector< int > planeIndexVector;
		for( int y = 1; y < subCubeMatrixSize / 2; y++ )
		{
			subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, y, subCubeMatrixSize - 1 ), *bestPerspective );
			if( subCube->faceData[ rightFace ].color != rightColor )
				planeIndexVector.push_back( y );
		}

		if( planeIndexVector.size() > 0 )
			return GenerateEdgeParityFixSequence( rubiksCube, *bestPerspective, planeIndexVector, rotationSequence );

		SetState( STAGE_COMPLETE );
		return false;
	}

	RubiksCube::SubCubeVector edgeCubies;
	rubiksCube->CollectSubCubes( colors, 2, edgeCubies );
	if( edgeCubies.size() != subCubeMatrixSize - 2 )
		return false;

	// Find the edge cube that we'll move into the partially completed edge.
	const RubiksCube::SubCube* edgeCube = 0;
	RubiksCube::Coordinates edgeCubeCoords;
	int i;
	for( i = 0; i < ( int )edgeCubies.size(); i++ )
	{
		edgeCube = edgeCubies[i];
		rubiksCube->RelativeFromActual( edgeCube->coords, edgeCubeCoords, *bestPerspective );
		if( edgeCubeCoords.x != subCubeMatrixSize - 1 || edgeCubeCoords.z != subCubeMatrixSize - 1 )
			break;
	}
	if( i == edgeCubies.size() )
		return false;

	RubiksCube::RelativeRotation relativeRotation;
	relativeRotation.planeIndex = 0;
	RubiksCube::Rotation rotation;
	int yCoordWhenLanded = -1;

	// Make the rotations that put the edge cube into the opposite edge.
	if( edgeCubeCoords.y == subCubeMatrixSize - 1 )
	{
		if( edgeCubeCoords.x == 0 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::Li;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = edgeCubeCoords.z;
		}
		else if( edgeCubeCoords.x == subCubeMatrixSize - 1 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::Ui;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			relativeRotation.type = RubiksCube::RelativeRotation::B;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = edgeCubeCoords.z;
		}
		else if( edgeCubeCoords.z == 0 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::B;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = edgeCubeCoords.x;
		}
		else if( edgeCubeCoords.z == subCubeMatrixSize - 1 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::U;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			relativeRotation.type = RubiksCube::RelativeRotation::Li;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = edgeCubeCoords.x;
		}
		else
		{
			// Something is wrong with the cube.
			return false;
		}
	}
	else if( edgeCubeCoords.y == 0 )
	{
		if( edgeCubeCoords.x == 0 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::L;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = subCubeMatrixSize - 1 - edgeCubeCoords.z;
		}
		else if( edgeCubeCoords.x == subCubeMatrixSize - 1 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::D;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			relativeRotation.type = RubiksCube::RelativeRotation::Bi;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = subCubeMatrixSize - 1 - edgeCubeCoords.z;
		}
		else if( edgeCubeCoords.z == 0 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::Bi;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			yCoordWhenLanded = subCubeMatrixSize - 1 - edgeCubeCoords.x;
		}
		else if( edgeCubeCoords.z == subCubeMatrixSize - 1 )
		{
			relativeRotation.type = RubiksCube::RelativeRotation::Di;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );

			relativeRotation.type = RubiksCube::RelativeRotation::L;
			rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
			rotationSequence.push_back( rotation );
			
			yCoordWhenLanded = subCubeMatrixSize - 1 - edgeCubeCoords.x;
		}
		else
		{
			// Something is wrong with the cube.
			return false;
		}
	}
	else if( edgeCubeCoords.z == 0 && edgeCubeCoords.x == subCubeMatrixSize - 1 )
	{
		relativeRotation.type = RubiksCube::RelativeRotation::B;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

		yCoordWhenLanded = subCubeMatrixSize - 1 - edgeCubeCoords.y;
	}
	else if( edgeCubeCoords.x == 0 && edgeCubeCoords.z == subCubeMatrixSize - 1 )
	{
		relativeRotation.type = RubiksCube::RelativeRotation::Li;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

		yCoordWhenLanded = subCubeMatrixSize - 1 - edgeCubeCoords.y;
	}
	else if( edgeCubeCoords.x == 0 && edgeCubeCoords.z == 0 )
	{
		// It's already landed.
		yCoordWhenLanded = edgeCubeCoords.y;
	}
	else
	{
		// Something's wrong with the cube.
		return false;
	}

	subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, subCubeMatrixSize - 1 - yCoordWhenLanded, subCubeMatrixSize - 1 ), *bestPerspective );
	if( ( subCube->faceData[ rightFace ].color != colors[0] || subCube->faceData[ forwardFace ].color != colors[1] ) &&
		( subCube->faceData[ rightFace ].color != colors[1] || subCube->faceData[ forwardFace ].color != colors[0] ) )
	{
		relativeRotation.type = RubiksCube::RelativeRotation::D;
		relativeRotation.planeIndex = yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

		// Flip the edge up-side down.
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		rubiksCube->ParseRelativeRotationSequenceString( "Fi, R, Ui, F, Ri", relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( *bestPerspective, relativeRotationSequence, rotationSequence );

		relativeRotation.type = RubiksCube::RelativeRotation::D;
		relativeRotation.planeIndex = yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

		return true;
	}

	subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, yCoordWhenLanded, subCubeMatrixSize - 1 ), *bestPerspective );
	if( ( subCube->faceData[ rightFace ].color != colors[0] || subCube->faceData[ forwardFace ].color != colors[1] ) &&
		( subCube->faceData[ rightFace ].color != colors[1] || subCube->faceData[ forwardFace ].color != colors[0] ) )
	{
		relativeRotation.type = RubiksCube::RelativeRotation::B;
		relativeRotation.planeIndex = 0;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

		relativeRotation.type = RubiksCube::RelativeRotation::Di;
		relativeRotation.planeIndex = subCubeMatrixSize - 1 - yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );

		//Flip the edge up-side down.
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		rubiksCube->ParseRelativeRotationSequenceString( "Fi, R, Ui, F, Ri", relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( *bestPerspective, relativeRotationSequence, rotationSequence );

		relativeRotation.type = RubiksCube::RelativeRotation::D;
		relativeRotation.planeIndex = subCubeMatrixSize - 1 - yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );

		return true;
	}

	// There is something wrong with the cube.
	return false;
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::EdgeSolver::VerifyActuallySolved( const RubiksCube* rubiksCube )
{
	return true;
}

//==================================================================================================
SolverForCaseGreaterThan3::As3x3x3Solver::As3x3x3Solver( void )
{
}

//==================================================================================================
/*virtual*/ SolverForCaseGreaterThan3::As3x3x3Solver::~As3x3x3Solver( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::As3x3x3Solver::SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();
	if( subCubeMatrixSize <= 3 )
		return false;

	RubiksCube* reducedRubiksCube = new RubiksCube( 3, false );

	for( int x = 0; x < 3; x++ )
	{
		for( int y = 0; y < 3; y++ )
		{
			for( int z = 0; z < 3; z++ )
			{
				RubiksCube::Coordinates coords( x, y, z );
				RubiksCube::SubCube* targetSubCube = reducedRubiksCube->Matrix( coords );
				
				if( x == 2 )
					coords.x = subCubeMatrixSize - 1;
				if( y == 2 )
					coords.y = subCubeMatrixSize - 1;
				if( z == 2 )
					coords.z = subCubeMatrixSize - 1;

				const RubiksCube::SubCube* subCube = rubiksCube->Matrix( coords );
				
				for( int i = 0; i < RubiksCube::CUBE_FACE_COUNT; i++ )
					targetSubCube->faceData[i].color = subCube->faceData[i].color;
			}
		}
	}

	SolverForCase3::ParityErrorList parityErrorList;
	SolverForCase3 solverForCase3( &parityErrorList );
	RubiksCube::RotationSequence reducedRotationSequence;
	solverForCase3.MakeEntireSolutionSequence( reducedRubiksCube, reducedRotationSequence );
	delete reducedRubiksCube;

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
				// it needs only rotate the outer planes to solve the puzzle, but account
				// for this anyway.
				for( int index = 1; index < subCubeMatrixSize - 1; index++ )
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
				rotation.plane.index = subCubeMatrixSize - 1;
				rotationSequence.push_back( rotation );
				break;
			}
		}
	}

	if( parityErrorList.size() > 0 && subCubeMatrixSize % 2 == 1 )
	{
		// Something went wrong.  We should never run into parity errors solving cubes of odd order as 3x3x3s.
		return false;
	}

	for( SolverForCase3::ParityErrorList::iterator iter = parityErrorList.begin(); iter != parityErrorList.end(); iter++ )
	{
		SolverForCase3::ParityError parityError = *iter;

		switch( parityError )
		{
			case SolverForCase3::ERROR_PARITY_FIX_WITH_EDGE_FLIP:
			{
				// TODO: Find the edge that needs to be flipped and flip it.
				break;
			}
			case SolverForCase3::ERROR_PARITY_FIX_WITH_EDGE_SWAP:
			{
				// TODO: Swap any two opposite edges in the -z face.
				break;
			}
		}
	}

	if( parityErrorList.size() > 0 )
		return true;
	
	SetState( STAGE_COMPLETE );
	return false;
}

// SolverForCaseGreaterThan3.cpp