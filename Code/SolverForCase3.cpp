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

	typedef void ( SolverForCase3::* PerformStageFunc )( const RubiksCube*, RubiksCube::RotationSequence& );
	PerformStageFunc performStageFuncArray[3] =
	{
		&SolverForCase3::PerformCubeOrientingStage,
		&SolverForCase3::PerformRedCrossPositioningStage,
		&SolverForCase3::PerformRedCrossOrientingStage,
		//...
	};

	int stageCount = sizeof( performStageFuncArray ) / sizeof( PerformStageFunc );
	for( int stage = 0; stage < stageCount; stage++ )
	{
		// A stage function won't generate a sequence in the case that its stage is complete.
		PerformStageFunc performStageFunc = performStageFuncArray[ stage ];
		( this->*performStageFunc )( rubiksCube, rotationSequence );
		if( rotationSequence.size() > 0 )
			return true;
	}

	return false;
}

//==================================================================================================
// A human would not need to perform this stage, but the computer,
// (or, rather, the programmer), needs a consistent frame of reference
// for the cube in all cases.
void SolverForCase3::PerformCubeOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	RubiksCube::SubCubeList redSubCube;
	RubiksCube::Color red = RubiksCube::RED;
	rubiksCube->CollectSubCubes( &red, 1, redSubCube );
	wxASSERT( redSubCube.size() == 1 );

	if( redSubCube.front()->z == 0 )
	{
		RubiksCube::Rotation rotation;
		rotation.plane.axis = RubiksCube::Y_AXIS;
		rotation.plane.index = 1;
		rotation.angle = M_PI;
		rotationSequence.push_back( rotation );
	}
	else if( redSubCube.front()->z == 1 )
	{
		RubiksCube::Rotation rotation;
		rotation.plane.index = 1;
		if( redSubCube.front()->y == 1 )
		{
			rotation.plane.axis = RubiksCube::Y_AXIS;
			rotation.angle = ( ( redSubCube.front()->x == 2 ) ? -M_PI : M_PI ) / 2.0;
		}
		else if( redSubCube.front()->x == 1 )
		{
			rotation.plane.axis = RubiksCube::X_AXIS;
			rotation.angle = ( ( redSubCube.front()->y == 2 ) ? M_PI : -M_PI ) / 2.0;
		}
		rotationSequence.push_back( rotation );
	}
	else
	{
		// If the red center sub-cube is in position, we can locate the other center sub-cubes.
		RubiksCube::SubCubeList yellowSubCube;
		RubiksCube::Color yellow = RubiksCube::YELLOW;
		rubiksCube->CollectSubCubes( &yellow, 1, yellowSubCube );
		wxASSERT( yellowSubCube.front()->z == 1 );

		if( yellowSubCube.front()->x != 2 )
		{
			// Notice that this rotation will necessarily put the
			// blue center sub-cube into its position.
			RubiksCube::Rotation rotation;
			rotation.plane.index = 1;
			rotation.plane.axis = RubiksCube::Z_AXIS;
			if( yellowSubCube.front()->y == 2 )
				rotation.angle = -M_PI / 2.0;
			else if( yellowSubCube.front()->y == 0 )
				rotation.angle = M_PI / 2.0;
			else
				rotation.angle = M_PI;
			rotationSequence.push_back( rotation );
		}
	}
}

//==================================================================================================
void SolverForCase3::PerformRedCrossPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	RubiksCube::Color redEdgeColors[4][2] =
	{
		RubiksCube::RED, RubiksCube::YELLOW,
		RubiksCube::RED, RubiksCube::GREEN,
		RubiksCube::RED, RubiksCube::WHITE,
		RubiksCube::RED, RubiksCube::BLUE,
	};

	int redEdgeTargetLocations[4][3] =
	{
		{ 2, 1, 2 },
		{ 1, 0, 2 },
		{ 0, 1, 2 },
		{ 1, 2, 2 },
	};

	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	RubiksCube::Perspective redEdgePerspectives[4];

	redEdgePerspectives[0].rAxis = xAxis;
	redEdgePerspectives[0].uAxis = zAxis;
	redEdgePerspectives[0].fAxis = -yAxis;

	redEdgePerspectives[1].rAxis = -yAxis;
	redEdgePerspectives[1].uAxis = zAxis;
	redEdgePerspectives[1].fAxis = -xAxis;

	redEdgePerspectives[2].rAxis = -xAxis;
	redEdgePerspectives[2].uAxis = zAxis;
	redEdgePerspectives[2].fAxis = yAxis;

	redEdgePerspectives[3].rAxis = yAxis;
	redEdgePerspectives[3].uAxis = zAxis;
	redEdgePerspectives[3].fAxis = xAxis;

	// Notice that if we produce redundant rotation with this algorithm,
	// or any algorithm that's part of the entire solving strategy, that
	// this is okay in the interests of simplifying the code, because the
	// calling code will run a compression pass on the sequence that we
	// return here, which will filter out such redundancies.
	for( int edge = 0; edge < 4; edge++ )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->CollectSubCube( redEdgeColors[ edge ], 2 );
		wxASSERT( subCube != 0 );
		
		int* targetLocation = redEdgeTargetLocations[ edge ];
		if( subCube->x == targetLocation[0] && subCube->y == targetLocation[1] && subCube->z == targetLocation[2] )
			continue;

		RubiksCube::Plane plane;
		if( subCube->x == 2 && ( subCube->y == 1 || subCube->y == 2 ) )
		{
			plane.axis = RubiksCube::X_AXIS;
			plane.index = 2;
		}
		else if( ( subCube->x == 0 || subCube->x == 1 ) && subCube->y == 2 )
		{
			plane.axis = RubiksCube::Y_AXIS;
			plane.index = 2;
		}
		else if( subCube->x == 0 && ( subCube->y == 0 || subCube->y == 1 ) )
		{
			plane.axis = RubiksCube::X_AXIS;
			plane.index = 0;
		}
		else if( ( subCube->x == 1 || subCube->x == 2 ) && subCube->y == 0 )
		{
			plane.axis = RubiksCube::Y_AXIS;
			plane.index = 0;
		}
		else
		{
			wxASSERT( false );
		}

		RubiksCube::Rotation rotation;
		RubiksCube::Rotation restorativeRotation;
		restorativeRotation.angle = 0.0;

		// Get the sub-cube into the Z=0 plane if it is not there already.
		if( subCube->z == 2 )
		{
			rotation.plane = plane;
			rotation.angle = M_PI;
			rotationSequence.push_back( rotation );
		}
		else if( subCube->z == 1 )
		{
			rotation.plane = plane;
			rotation.angle = M_PI / 2.0;
			if( ( subCube->x == 0 && subCube->y == 0 ) || ( subCube->x == 2 && subCube->y == 2 ) )
				rotation.angle *= -1.0;
			rotationSequence.push_back( rotation );
			restorativeRotation.angle = -rotation.angle;
			restorativeRotation.plane = plane;
		}

		// Rotate the sub-cube in the Z=0 plane so that it is ready to be rotated into position.
		rotation.plane.axis = RubiksCube::Z_AXIS;
		rotation.plane.index = 0;
		rotation.angle = 0.0;
		c3ga::vectorE3GA axis = RubiksCube::TranslateAxis( plane.axis );
		if( plane.index == 0 )
			axis = -axis;
		RubiksCube::Perspective* perspective = &redEdgePerspectives[ edge ];
		double dot = c3ga::lc( axis, perspective->rAxis );
		double epsilon = 1e-7;
		if( fabs( dot + 1.0 ) < epsilon )
			rotation.angle = M_PI;
		else if( fabs( dot - 1.0 ) >= epsilon )
		{
			rotation.angle = M_PI / 2.0;
			c3ga::trivectorE3GA trivector = perspective->rAxis ^ zAxis ^ axis;
			rotation.angle *= trivector.get_e1_e2_e3();
		}
		if( rotation.angle != 0.0 )
			rotationSequence.push_back( rotation );
		else
		{
			// No restoration is needed, (or wanted), if we were already in the
			// right plane, ready to be moved up into position.
			restorativeRotation.angle = 0.0;
		}

		// At this point we can restore what we may have already solved.
		if( restorativeRotation.angle != 0.0 )
		{
			// TODO: We might consider for-going this restorative move if, by chance,
			//       the rotation above causing us to need this actually put the right
			//       edge cube into its target location.
			rotationSequence.push_back( restorativeRotation );
		}

		// Finally, rotate the sub-cube into its proper position.  :)
		RubiksCube::RelativeRotation relativeRotation = RubiksCube::R;
		rubiksCube->TranslateRotation( *perspective, relativeRotation, rotation );
		rotation.angle = M_PI;
		rotationSequence.push_back( rotation );
		
		// We can't go onto to solve the next edge at this time, because its position
		// in the cube may have been changed by the sequence of moves we have just made.
		break;
	}
}

//==================================================================================================
void SolverForCase3::PerformRedCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	// If the Red/Yellow edge piece is not oriented properly,
	//   choose rAxis = xAxis, uAxis = zAxis, fAxis = -yAxis, then do Ri, U, Fi, Ui.
	// If the Red/Blue edge piece is not oriented properly,
	//   choose rAxis = yAxis, uAxis = zAxis, fAxis = xAxis, then do Ri, U, Fi, Ui.
	// If the Red/White edge piece is not oriented properly,
	//   choose rAxis = -xAxis, uAxis = zAxis, fAxis = yAxis, then do Ri, U, Fi, Ui.
	// If the Red/Green edge piece is not oriented properly,
	//   choose rAxis = -yAxis, uAxis = zAxis, fAxis = -xAxis, then do Ri, U, Fi, Ui.
}

//void SolverForCase3::PerformRedFaceCornersPositioningStage()
//{
	// For each corner piece,
	//   If it is not already in position,
	//     If it is in the Z=2 plane, do a sequence to get it out to Z=0 plane without disturbing what we have
	//     If it is in Z=0 plane, but not the right corner, get it into the right corner
	//     If it is in Z=0 plane and the right corner, sequence it up into position.
//}

//void SolverFromCase3::PerformRedFaceCornerOrientingStage()
//{
	// For each corner piece,
	//   If not already in right orientation
	//     Choose correct perspective, then do (Ri, Di, R, D)x2
//}

// SolverForCase3.cpp