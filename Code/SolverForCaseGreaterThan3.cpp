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
	stageSolverList.push_back( new FaceSolver( RubiksCube::NEG_Z ) );

	stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::BLUE ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::RED ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::GREEN ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::YELLOW, RubiksCube::ORANGE ) );

	//stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::BLUE ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::RED ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::GREEN ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::WHITE, RubiksCube::ORANGE ) );

	//stageSolverList.push_back( new EdgeSolver( RubiksCube::BLUE, RubiksCube::ORANGE ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::ORANGE, RubiksCube::GREEN ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::GREEN, RubiksCube::RED ) );
	//stageSolverList.push_back( new EdgeSolver( RubiksCube::RED, RubiksCube::BLUE ) );
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
	RubiksCube::Color color = RubiksCube::TranslateFaceColor( face );

	PerspectiveList::iterator iter = perspectiveList.begin();
	iter++;

	RubiksCube::Perspective perspective = *iter;

	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();

	for( int x = 1; x < subCubeMatrixSize - 1; x++ )
	{
		for( int z = 1; z < subCubeMatrixSize - 1; z++ )
		{
			RubiksCube::Coordinates upwardCoords;
			upwardCoords.x = x;
			upwardCoords.y = subCubeMatrixSize - 1;
			upwardCoords.z = z;

			const RubiksCube::SubCube* subCube = rubiksCube->Matrix( upwardCoords, perspective );
			RubiksCube::Face upwardFace = RubiksCube::TranslateNormal( perspective.uAxis );
			if( subCube->faceData[ upwardFace ].color != color )
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
	rotationSequence.push_back( invRotation );		// Shouldn't need this one, but whatever.

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
	perspective.rAxis = yAxis;
	perspective.rAxis = zAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = zAxis;
	perspective.rAxis = yAxis;
	perspective.rAxis = -xAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -xAxis;
	perspective.rAxis = yAxis;
	perspective.rAxis = -zAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -zAxis;
	perspective.rAxis = yAxis;
	perspective.rAxis = xAxis;
	perspectiveList.push_back( perspective );

	//---

	perspective.rAxis = xAxis;
	perspective.rAxis = -zAxis;
	perspective.rAxis = yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = yAxis;
	perspective.rAxis = -zAxis;
	perspective.rAxis = -xAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = zAxis;
	perspective.rAxis = xAxis;
	perspective.rAxis = yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = yAxis;
	perspective.rAxis = xAxis;
	perspective.rAxis = -zAxis;
	perspectiveList.push_back( perspective );

	//---

	perspective.rAxis = zAxis;
	perspective.rAxis = -xAxis;
	perspective.rAxis = -yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -yAxis;
	perspective.rAxis = -xAxis;
	perspective.rAxis = -zAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = xAxis;
	perspective.rAxis = zAxis;
	perspective.rAxis = -yAxis;
	perspectiveList.push_back( perspective );

	perspective.rAxis = -yAxis;
	perspective.rAxis = zAxis;
	perspective.rAxis = -xAxis;
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

		int edgeCount = 0;
		for( int y = 1; y < subCubeMatrixSize - 1; y++ )
		{
			RubiksCube::Coordinates coords;
			coords.x = subCubeMatrixSize - 1;
			coords.y = y;
			coords.z = subCubeMatrixSize - 1;

			subCube = rubiksCube->Matrix( coords, perspective );
			
			rightFace = RubiksCube::TranslateNormal( perspective.rAxis );
			forwardFace = RubiksCube::TranslateNormal( perspective.fAxis );

			if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
				( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
			{
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

	if( biggestEdgeCount == subCubeMatrixSize - 2 )
	{
		SetState( STAGE_COMPLETE );
		return true;
	}

	rightFace = RubiksCube::TranslateNormal( bestPerspective->rAxis );
	forwardFace = RubiksCube::TranslateNormal( bestPerspective->fAxis );

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
		rubiksCube->RelativeFromActual( subCube->coords, edgeCubeCoords, *bestPerspective );
		if( edgeCubeCoords.x != subCubeMatrixSize - 1 && edgeCubeCoords.z != subCubeMatrixSize - 1 )
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

	subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, yCoordWhenLanded, subCubeMatrixSize - 1 ), *bestPerspective );
	if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
		( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
	{
		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, subCubeMatrixSize - 1 - yCoordWhenLanded, subCubeMatrixSize - 1 ), *bestPerspective );
		if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
			( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
		{
			// Something is wrong with the cube.
			return false;
		}

		relativeRotation.type = RubiksCube::RelativeRotation::D;
		relativeRotation.planeIndex = yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

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

	subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, subCubeMatrixSize - 1 - yCoordWhenLanded, subCubeMatrixSize - 1 ), *bestPerspective );
	if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
		( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
	{
		subCube = rubiksCube->Matrix( RubiksCube::Coordinates( subCubeMatrixSize - 1, yCoordWhenLanded, subCubeMatrixSize - 1 ), *bestPerspective );
		if( ( subCube->faceData[ rightFace ].color == colors[0] && subCube->faceData[ forwardFace ].color == colors[1] ) ||
			( subCube->faceData[ rightFace ].color == colors[1] && subCube->faceData[ forwardFace ].color == colors[0] ) )
		{
			// Something is wrong with the cube.
			return false;
		}

		relativeRotation.type = RubiksCube::RelativeRotation::B;
		relativeRotation.planeIndex = 0;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( rotation );

		relativeRotation.type = RubiksCube::RelativeRotation::Di;
		relativeRotation.planeIndex = subCubeMatrixSize - 1 - yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );

		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		rubiksCube->ParseRelativeRotationSequenceString( "Fi, R, Ui, F, Ri", relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( *bestPerspective, relativeRotationSequence, rotationSequence );

		relativeRotation.type = RubiksCube::RelativeRotation::D;
		relativeRotation.planeIndex = subCubeMatrixSize - 1 - yCoordWhenLanded;
		rubiksCube->TranslateRotation( *bestPerspective, relativeRotation, rotation );
		rotationSequence.push_back( rotation );

		return true;
	}

	return false;
}

//==================================================================================================
/*virtual*/ bool SolverForCaseGreaterThan3::EdgeSolver::VerifyActuallySolved( const RubiksCube* rubiksCube )
{
	return true;
}

// SolverForCaseGreaterThan3.cpp