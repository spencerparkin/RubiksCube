// SolverForCase3.cpp

#include "Header.h"

c3ga::vectorE3GA SolverForCase3::xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
c3ga::vectorE3GA SolverForCase3::yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
c3ga::vectorE3GA SolverForCase3::zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

RubiksCube::Perspective SolverForCase3::redEdgeOrCornerPerspectives[4];

RubiksCube::Color SolverForCase3::redEdgeColors[4][2] =
{
	RubiksCube::RED, RubiksCube::YELLOW,
	RubiksCube::RED, RubiksCube::GREEN,
	RubiksCube::RED, RubiksCube::WHITE,
	RubiksCube::RED, RubiksCube::BLUE,
};

int SolverForCase3::redEdgeTargetLocations[4][3] =
{
	{ 2, 1, 2 },
	{ 1, 0, 2 },
	{ 0, 1, 2 },
	{ 1, 2, 2 },
};

RubiksCube::Color SolverForCase3::redCornerColors[4][3] =
{
	RubiksCube::RED, RubiksCube::YELLOW, RubiksCube::GREEN,
	RubiksCube::RED, RubiksCube::GREEN, RubiksCube::WHITE,
	RubiksCube::RED, RubiksCube::WHITE, RubiksCube::BLUE,
	RubiksCube::RED, RubiksCube::BLUE, RubiksCube::YELLOW,
};

int SolverForCase3::redCornerTargetLocations[4][3] =
{
	{ 2, 0, 2 },
	{ 0, 0, 2 },
	{ 0, 2, 2 },
	{ 2, 2, 2 },
};

//==================================================================================================
SolverForCase3::SolverForCase3( void )
{
	redEdgeOrCornerPerspectives[0].rAxis = xAxis;
	redEdgeOrCornerPerspectives[0].uAxis = zAxis;
	redEdgeOrCornerPerspectives[0].fAxis = -yAxis;

	redEdgeOrCornerPerspectives[1].rAxis = -yAxis;
	redEdgeOrCornerPerspectives[1].uAxis = zAxis;
	redEdgeOrCornerPerspectives[1].fAxis = -xAxis;

	redEdgeOrCornerPerspectives[2].rAxis = -xAxis;
	redEdgeOrCornerPerspectives[2].uAxis = zAxis;
	redEdgeOrCornerPerspectives[2].fAxis = yAxis;

	redEdgeOrCornerPerspectives[3].rAxis = yAxis;
	redEdgeOrCornerPerspectives[3].uAxis = zAxis;
	redEdgeOrCornerPerspectives[3].fAxis = xAxis;
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
	PerformStageFunc performStageFuncArray[5] =
	{
		&SolverForCase3::PerformCubeOrientingStage,
		&SolverForCase3::PerformRedCrossPositioningStage,
		&SolverForCase3::PerformRedCrossOrientingStage,
		&SolverForCase3::PerformRedCornersPositioningStage,
		&SolverForCase3::PerformRedCornersOrientingStage,
	};

	int stageCount = sizeof( performStageFuncArray ) / sizeof( PerformStageFunc );
	for( int stage = 0; stage < stageCount; stage++ )
	{
		// A stage function won't generate a sequence in the case that its stage is complete.
		PerformStageFunc performStageFunc = performStageFuncArray[ stage ];
		( this->*performStageFunc )( rubiksCube, rotationSequence );
		if( rotationSequence.size() > 0 )
			break;
	}

	return true;
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
			if( plane.index == 2 )
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
		RubiksCube::Perspective* perspective = &redEdgeOrCornerPerspectives[ edge ];
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
	for( int edge = 0; edge < 4; edge++ )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->CollectSubCube( redEdgeColors[ edge ], 2 );
		if( subCube->faceColor[ RubiksCube::POS_Z ] == RubiksCube::RED )
			continue;

		RubiksCube::Perspective* perspective = &redEdgeOrCornerPerspectives[ edge ];
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		relativeRotationSequence.push_back( RubiksCube::Ri );
		relativeRotationSequence.push_back( RubiksCube::U );
		relativeRotationSequence.push_back( RubiksCube::Fi );
		relativeRotationSequence.push_back( RubiksCube::Ui );
		rubiksCube->TranslateRotationSequence( *perspective, relativeRotationSequence, rotationSequence );
		break;
	}
}

//==================================================================================================
void SolverForCase3::PerformRedCornersPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	for( int corner = 0; corner < 4; corner++ )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->CollectSubCube( redCornerColors[ corner ], 3 );
		wxASSERT( subCube != 0 );

		int* targetLocation = redCornerTargetLocations[ corner ];
		if( subCube->x == targetLocation[0] && subCube->y == targetLocation[1] && subCube->z == targetLocation[2] )
			continue;

		RubiksCube::Rotation rotation;
		RubiksCube::Rotation restorativeRotation;
		restorativeRotation.angle = 0.0;

		int foundCorner;
		for( foundCorner = 0; foundCorner < 4; foundCorner++ )
		{
			int* location = redCornerTargetLocations[ foundCorner ];
			if( subCube->x == location[0] && subCube->y == location[1] )
				break;
		}
		wxASSERT( foundCorner != 4 );
		RubiksCube::Perspective* foundPerspective = &redEdgeOrCornerPerspectives[ foundCorner ];

		// Get the sub-cube into the Z=0 plane if it is not there already.
		if( subCube->z == 2 )
		{
			rubiksCube->TranslateRotation( *foundPerspective, RubiksCube::Ri, rotation );
			rubiksCube->TranslateRotation( *foundPerspective, RubiksCube::R, restorativeRotation );
			rotationSequence.push_back( rotation );
		}

		// Rotate the sub-cube in the Z=0 plane so that it is ready to be rotated into position.
		rotation.plane.axis = RubiksCube::Z_AXIS;
		rotation.plane.index = 0;
		rotation.angle = 0.0;
		RubiksCube::Perspective* perspective = &redEdgeOrCornerPerspectives[ corner ];
		double dot = c3ga::lc( perspective->rAxis, foundPerspective->rAxis );
		double epsilon = 1e-7;
		if( fabs( dot + 1.0 ) < epsilon )
			rotation.angle = M_PI;
		else if( fabs( dot - 1.0 ) >= epsilon )
		{
			rotation.angle = M_PI / 2.0;
			c3ga::trivectorE3GA trivector = foundPerspective->rAxis ^ zAxis ^ perspective->rAxis;
			rotation.angle *= -trivector.get_e1_e2_e3();
		}
		if( rotation.angle != 0.0 )
			rotationSequence.push_back( rotation );

		// At this point we can restore what we may have already solved.
		if( restorativeRotation.angle != 0.0 )
			rotationSequence.push_back( restorativeRotation );

		// If we've made rotation up to this point, we need to flush those
		// out (get them applied), so that the logic to follow can have a
		// fix on the position and orientation of the corner.
		if( rotationSequence.size() > 0 )
			break;

		// Try to rotate the corner up into position so that it is also oriented correctly.
		// This is not always possible, so we will still need an orientation stage.
		wxASSERT( foundPerspective == perspective );
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		RubiksCube::Face rightFace = RubiksCube::TranslateNormal( perspective->rAxis );
		RubiksCube::Face downFace = RubiksCube::TranslateNormal( -perspective->uAxis );
		if( subCube->faceColor[ rightFace ] == RubiksCube::RED || subCube->faceColor[ downFace ] == RubiksCube::RED )
		{
			relativeRotationSequence.push_back( RubiksCube::D );
			relativeRotationSequence.push_back( RubiksCube::F );
			relativeRotationSequence.push_back( RubiksCube::Di );
			relativeRotationSequence.push_back( RubiksCube::Fi );
		}
		else
		{
			relativeRotationSequence.push_back( RubiksCube::Di );
			relativeRotationSequence.push_back( RubiksCube::Ri );
			relativeRotationSequence.push_back( RubiksCube::D );
			relativeRotationSequence.push_back( RubiksCube::R );
		}

		rubiksCube->TranslateRotationSequence( *perspective, relativeRotationSequence, rotationSequence );
		break;
	}
}

//==================================================================================================
void SolverForCase3::PerformRedCornersOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	for( int corner = 0; corner < 4; corner++ )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->CollectSubCube( redCornerColors[ corner ], 3 );
		wxASSERT( subCube != 0 );

		if( subCube->faceColor[ RubiksCube::POS_Z ] == RubiksCube::RED )
			continue;

		RubiksCube::Perspective* perspective = &redEdgeOrCornerPerspectives[ corner ];
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		relativeRotationSequence.push_back( RubiksCube::Ri );
		relativeRotationSequence.push_back( RubiksCube::Di );
		relativeRotationSequence.push_back( RubiksCube::R );
		relativeRotationSequence.push_back( RubiksCube::D );
		relativeRotationSequence.push_back( RubiksCube::Ri );
		relativeRotationSequence.push_back( RubiksCube::Di );
		relativeRotationSequence.push_back( RubiksCube::R );
		rubiksCube->TranslateRotationSequence( *perspective, relativeRotationSequence, rotationSequence );
		break;
	}
}

// SolverForCase3.cpp