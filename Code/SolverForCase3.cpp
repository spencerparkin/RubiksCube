// SolverForCase3.cpp

#include "Header.h"

c3ga::vectorE3GA SolverForCase3::xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
c3ga::vectorE3GA SolverForCase3::yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
c3ga::vectorE3GA SolverForCase3::zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

RubiksCube::Perspective SolverForCase3::standardPerspectives[4];
RubiksCube::Perspective SolverForCase3::standardPerspectivesNegated[4];

RubiksCube::Color SolverForCase3::redEdgeColors[4][2] =
{
	{ RubiksCube::RED, RubiksCube::YELLOW },
	{ RubiksCube::RED, RubiksCube::GREEN },
	{ RubiksCube::RED, RubiksCube::WHITE },
	{ RubiksCube::RED, RubiksCube::BLUE },
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
	{ RubiksCube::RED, RubiksCube::YELLOW, RubiksCube::GREEN },
	{ RubiksCube::RED, RubiksCube::GREEN, RubiksCube::WHITE },
	{ RubiksCube::RED, RubiksCube::WHITE, RubiksCube::BLUE },
	{ RubiksCube::RED, RubiksCube::BLUE, RubiksCube::YELLOW },
};

int SolverForCase3::redCornerTargetLocations[4][3] =
{
	{ 2, 0, 2 },
	{ 0, 0, 2 },
	{ 0, 2, 2 },
	{ 2, 2, 2 },
};

RubiksCube::Color SolverForCase3::middleEdgeColors[4][2] =
{
	{ RubiksCube::YELLOW, RubiksCube::GREEN },
	{ RubiksCube::BLUE, RubiksCube::YELLOW },
	{ RubiksCube::WHITE, RubiksCube::BLUE },
	{ RubiksCube::GREEN, RubiksCube::WHITE },
};

int SolverForCase3::middleEdgeTargetLocations[4][3] =
{
	{ 2, 0, 1 },
	{ 2, 2, 1 },
	{ 0, 2, 1 },
	{ 0, 0, 1 },
};

RubiksCube::Color SolverForCase3::orangeEdgeColors[4][2] =
{
	{ RubiksCube::ORANGE, RubiksCube::GREEN },
	{ RubiksCube::ORANGE, RubiksCube::YELLOW },
	{ RubiksCube::ORANGE, RubiksCube::BLUE },
	{ RubiksCube::ORANGE, RubiksCube::WHITE },
};

int SolverForCase3::orangeEdgeTargetLocations[4][3] =
{
	{ 1, 0, 0 },
	{ 2, 1, 0 },
	{ 1, 2, 0 },
	{ 0, 1, 0 },
};

RubiksCube::Color SolverForCase3::orangeCornerColors[4][3] =
{
	{ RubiksCube::ORANGE, RubiksCube::YELLOW, RubiksCube::GREEN },
	{ RubiksCube::ORANGE, RubiksCube::BLUE, RubiksCube::YELLOW },
	{ RubiksCube::ORANGE, RubiksCube::WHITE, RubiksCube::BLUE },
	{ RubiksCube::ORANGE, RubiksCube::GREEN, RubiksCube::WHITE },
};

int SolverForCase3::orangeCornerTargetLocations[4][3] =
{
	{ 2, 0, 0 },
	{ 2, 2, 0 },
	{ 0, 2, 0 },
	{ 0, 0, 0 },
};

//==================================================================================================
SolverForCase3::SolverForCase3( void )
{
	standardPerspectives[0].rAxis = xAxis;
	standardPerspectives[0].uAxis = zAxis;
	standardPerspectives[0].fAxis = -yAxis;

	standardPerspectives[1].rAxis = -yAxis;
	standardPerspectives[1].uAxis = zAxis;
	standardPerspectives[1].fAxis = -xAxis;

	standardPerspectives[2].rAxis = -xAxis;
	standardPerspectives[2].uAxis = zAxis;
	standardPerspectives[2].fAxis = yAxis;

	standardPerspectives[3].rAxis = yAxis;
	standardPerspectives[3].uAxis = zAxis;
	standardPerspectives[3].fAxis = xAxis;

	standardPerspectivesNegated[0].rAxis = -yAxis;
	standardPerspectivesNegated[0].uAxis = -zAxis;
	standardPerspectivesNegated[0].fAxis = xAxis;

	standardPerspectivesNegated[1].rAxis = xAxis;
	standardPerspectivesNegated[1].uAxis = -zAxis;
	standardPerspectivesNegated[1].fAxis = yAxis;

	standardPerspectivesNegated[2].rAxis = yAxis;
	standardPerspectivesNegated[2].uAxis = -zAxis;
	standardPerspectivesNegated[2].fAxis = -xAxis;

	standardPerspectivesNegated[3].rAxis = -xAxis;
	standardPerspectivesNegated[3].uAxis = -zAxis;
	standardPerspectivesNegated[3].fAxis = -yAxis;
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
	PerformStageFunc performStageFuncArray[10] =
	{
		&SolverForCase3::PerformCubeOrientingStage,
		&SolverForCase3::PerformRedCrossPositioningStage,
		&SolverForCase3::PerformRedCrossOrientingStage,
		&SolverForCase3::PerformRedCornersPositioningStage,
		&SolverForCase3::PerformRedCornersOrientingStage,
		&SolverForCase3::PerformMiddleEdgePositioningAndOrientingStage,
		&SolverForCase3::PerformOrangeCrossOrientingStage,
		&SolverForCase3::PerformOrangeCrossAndCornersRelativePositioningStage,
		&SolverForCase3::PerformOrangeCornerOrientingStage,
		&SolverForCase3::PerformOrangeCrossAndCornersPositioningStage,
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
		RubiksCube::Perspective* perspective = &standardPerspectives[ edge ];
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
		RubiksCube::RelativeRotation relativeRotation( RubiksCube::RelativeRotation::R );
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

		RubiksCube::Perspective* perspective = &standardPerspectives[ edge ];
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		RubiksCube::ParseRelativeRotationSequenceString( "Ri, U, Fi, Ui", relativeRotationSequence );
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
		RubiksCube::Perspective* foundPerspective = &standardPerspectives[ foundCorner ];

		// Get the sub-cube into the Z=0 plane if it is not there already.
		if( subCube->z == 2 )
		{
			rubiksCube->TranslateRotation( *foundPerspective, RubiksCube::RelativeRotation::Ri, rotation );
			rubiksCube->TranslateRotation( *foundPerspective, RubiksCube::RelativeRotation::R, restorativeRotation );
			rotationSequence.push_back( rotation );
		}

		// Rotate the sub-cube in the Z=0 plane so that it is ready to be rotated into position.
		rotation.plane.axis = RubiksCube::Z_AXIS;
		rotation.plane.index = 0;
		rotation.angle = 0.0;
		RubiksCube::Perspective* perspective = &standardPerspectives[ corner ];
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
			RubiksCube::ParseRelativeRotationSequenceString( "D, F, Di, Fi", relativeRotationSequence );
		else
			RubiksCube::ParseRelativeRotationSequenceString( "Di, Ri, D, R", relativeRotationSequence );
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

		RubiksCube::Perspective* perspective = &standardPerspectives[ corner ];
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		RubiksCube::ParseRelativeRotationSequenceString( "Ri, Di, R, D, Ri, Di, R", relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( *perspective, relativeRotationSequence, rotationSequence );
		break;
	}
}

//==================================================================================================
// Here we always look for middle edges in the Z=0 plane first and position/orient them first,
// because in doing so, it may bubble some of our wrongly positioned/oriented middle edges in
// the Z=1 plane up into the Z=0 plane where they are ready to be moved into where they should be.
// Only when we don't find any in the Z=0 plane do we go pull them out of the Z=1 plane and into
// the Z=0 plane.
void SolverForCase3::PerformMiddleEdgePositioningAndOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	for( int edge = 0; edge < 4; edge++ )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->CollectSubCube( middleEdgeColors[ edge ], 2 );

		if( subCube->z != 0 )
			continue;

		RubiksCube::Perspective* perspective = &standardPerspectivesNegated[ edge ];

		RubiksCube::Rotation rotation;
		rotation.plane.axis = RubiksCube::Z_AXIS;
		rotation.plane.index = 0;
		rotation.angle = 0.0;

		c3ga::vectorE3GA targetAxis;
		if( subCube->faceColor[ RubiksCube::NEG_Z ] == middleEdgeColors[ edge ][0] )
			targetAxis = perspective->rAxis;
		else if( subCube->faceColor[ RubiksCube::NEG_Z ] == middleEdgeColors[ edge ][1] )
			targetAxis = perspective->fAxis;
		else
			wxASSERT( false );

		c3ga::vectorE3GA axis( c3ga::vectorE3GA::coord_e1_e2_e3, double( subCube->x - 1 ), double( subCube->y - 1 ), 0.0 );
		double dot = c3ga::lc( axis, targetAxis );
		double epsilon = 1e-7;
		if( fabs( dot + 1.0 ) < epsilon )
			rotation.angle = M_PI;
		else if( fabs( dot - 1.0 ) >= epsilon )
		{
			rotation.angle = M_PI / 2.0;
			c3ga::trivectorE3GA trivector = axis ^ -zAxis ^ targetAxis;
			rotation.angle *= trivector.get_e1_e2_e3();
		}

		if( rotation.angle != 0.0 )
			rotationSequence.push_back( rotation );

		// Move the edge piece into the correct position and orientation.  :)
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		if( subCube->faceColor[ RubiksCube::NEG_Z ] == middleEdgeColors[ edge ][0] )
			RubiksCube::ParseRelativeRotationSequenceString( "Ui, Fi, U, F, U, R, Ui, Ri", relativeRotationSequence );
		else
			RubiksCube::ParseRelativeRotationSequenceString( "U, R, Ui, Ri, Ui, Fi, U, F", relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( *perspective, relativeRotationSequence, rotationSequence );

		break;
	}

	if( rotationSequence.size() > 0 )
		return;

	for( int edge = 0; edge < 4; edge++ )
	{
		const RubiksCube::SubCube* subCube = rubiksCube->CollectSubCube( middleEdgeColors[ edge ], 2 );
		wxASSERT( subCube->z == 1 );

		RubiksCube::Perspective* perspective = &standardPerspectivesNegated[ edge ];

		int* targetLocation = middleEdgeTargetLocations[ edge ];
		if( subCube->x == targetLocation[0] && subCube->y == targetLocation[1] && subCube->z == targetLocation[2] )
		{
			RubiksCube::Face forwardFace = RubiksCube::TranslateNormal( perspective->fAxis );
			RubiksCube::Face rightFace = RubiksCube::TranslateNormal( perspective->rAxis );
			
			if( subCube->faceColor[ forwardFace ] == middleEdgeColors[ edge ][0] && subCube->faceColor[ rightFace ] == middleEdgeColors[ edge ][1] )
				continue;
		}

		// Get the edge sub-cube up into the Z=0 plane.
		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		RubiksCube::ParseRelativeRotationSequenceString( "R, Ui, Ri, Ui, Fi, U, F", relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( *perspective, relativeRotationSequence, rotationSequence );

		break;
	}
}

//==================================================================================================
void SolverForCase3::PerformOrangeCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	int properOrientationCount = 0;
	bool orientedProperly[4];

	int edge;
	for( edge = 0; edge < 4; edge++ )
	{
		int* location = orangeEdgeTargetLocations[ edge ];
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( location[0], location[1], 0 );

		orientedProperly[ edge ] = false;
		if( subCube->faceColor[ RubiksCube::NEG_Z ] == RubiksCube::ORANGE )
		{
			orientedProperly[ edge ] = true;
			properOrientationCount++;
		}
	}

	RubiksCube::Perspective perspective;
	RubiksCube::RelativeRotationSequence relativeRotationSequence;

	enum Sequence
	{
		NO_SEQUENCE,
		SEQUENCE_NORMAL,
		SEQUENCE_INVERSE,
	};

	Sequence sequence = NO_SEQUENCE;

	switch( properOrientationCount )
	{
		case 0:		// Take us to case 2.
		{
			sequence = SEQUENCE_NORMAL;
			perspective = standardPerspectivesNegated[0];	// Choose any of the 4 perspectives arbitrarily.
			break;
		}
		case 1:		// Take us to case 2.
		{
			sequence = SEQUENCE_NORMAL;
			for( edge = 0; edge < 4; edge++ )
			{
				if( orientedProperly[ edge ] )
				{
					perspective = standardPerspectivesNegated[ edge ];
					break;
				}
			}
			wxASSERT( edge < 4 );
			break;
		}
		case 2:		// Take us to case 4.
		{
			if( orientedProperly[0] && orientedProperly[2] )
			{
				sequence = SEQUENCE_NORMAL;
				perspective = standardPerspectivesNegated[0];	// Choose between 2 of the 4 perspectives arbitrarily.
			}
			else if( orientedProperly[1] && orientedProperly[3] )
			{
				sequence = SEQUENCE_NORMAL;
				perspective = standardPerspectivesNegated[1];	// Choose between 2 of the 4 perspectives arbitrarily.
			}
			else
			{
				sequence = SEQUENCE_INVERSE;
				for( edge = 0; edge < 4; edge++ )
				{
					if( orientedProperly[ edge ] && orientedProperly[ ( edge + 1 ) % 4 ] )
					{
						perspective = standardPerspectivesNegated[ ( edge + 2 ) % 4 ];	// Choose the only correct perspective.
						break;
					}
				}
				wxASSERT( edge < 4 );
			}

			break;
		}
		case 3:		// Take us to case 1.
		{
			sequence = SEQUENCE_NORMAL;
			for( edge = 0; edge < 4; edge++ )
			{
				if( !orientedProperly[ edge ] && orientedProperly[ ( edge + 1 ) % 4 ] )
				{
					perspective = standardPerspectivesNegated[ ( edge + 2 ) % 4 ];
					break;
				}
			}
			wxASSERT( edge < 4 );
			break;
		}
		case 4:		// In this case, our work is done!
		{
			sequence = NO_SEQUENCE;
			break;
		}
	}

	switch( sequence )
	{
		case SEQUENCE_NORMAL:
		{
			RubiksCube::ParseRelativeRotationSequenceString( "F, R, U, Ri, Ui, Fi", relativeRotationSequence );
			break;
		}
		case SEQUENCE_INVERSE:
		{
			RubiksCube::ParseRelativeRotationSequenceString( "F, U, R, Ui, Ri, Fi", relativeRotationSequence );
			break;
		}
	}

	if( relativeRotationSequence.size() > 0 )
		rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, rotationSequence );
}

//==================================================================================================
void SolverForCase3::PerformOrangeCrossAndCornersRelativePositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	int edgeQuad[4];
	for( int edge = 0; edge < 4; edge++ )
	{
		int* location = orangeEdgeTargetLocations[ edge ];
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( location[0], location[1], 0 );

		c3ga::vectorE3GA normal( c3ga::vectorE3GA::coord_e1_e2_e3, double( location[0] - 1 ), double( location[1] - 1 ), 0.0 );
		RubiksCube::Face face = RubiksCube::TranslateNormal( normal );
		RubiksCube::Color color = subCube->faceColor[ face ];

		int index;
		for( index = 0; index < 4; index++ )
			if( orangeEdgeColors[ index ][1] == color )
				break;

		wxASSERT( index != 4 );
		edgeQuad[ edge ] = index;
	}

	if( !TriCycleSolver::IsQuadInOrder( edgeQuad ) )
	{
		TriCycleSolver::TriCycleSequence triCycleSequence;
		bool found = TriCycleSolver::FindSmallestTriCycleSequenceThatOrdersQuad( edgeQuad, triCycleSequence );
		wxASSERT( found );

		for( TriCycleSolver::TriCycleSequence::iterator iter = triCycleSequence.begin(); iter != triCycleSequence.end(); iter++ )
		{
			TriCycleSolver::TriCycle triCycle = *iter;

			RubiksCube::RelativeRotationSequence relativeRotationSequence;
			RubiksCube::Perspective perspective;

			switch( triCycle.direction )
			{
				case TriCycleSolver::TriCycle::FORWARD:
				{
					RubiksCube::ParseRelativeRotationSequenceString( "R, Ui, Ui, Ri, Ui, R, Ui, R", relativeRotationSequence );
					perspective = standardPerspectivesNegated[ ( triCycle.invariantIndex + 3 ) % 4 ];
					break;
				}
				case TriCycleSolver::TriCycle::BACKWARD:
				{
					RubiksCube::ParseRelativeRotationSequenceString( "R, U, Ri, U, R, U, U, Ri", relativeRotationSequence );
					perspective = standardPerspectivesNegated[ ( triCycle.invariantIndex + 3 ) % 4 ];
					break;
				}
			}

			if( relativeRotationSequence.size() > 0 )
				rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, rotationSequence );
		}

		return;
	}

	// Notice that we must consider the corner relative positioning after that of the edges,
	// because the rotation sequences we use to position the corners leaves the edges invariant,
	// but the opposite is not true.  That is, the sequences we use to position the edges do
	// not leave the corner positions invariant.
	
	int cornerQuad[4];
	for( int corner = 0; corner < 4; corner++ )
	{
		int* location = orangeCornerTargetLocations[ corner ];
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( location[0], location[1], 0 );

		int index;
		for( index = 0; index < 4; index++ )
			if( RubiksCube::CubeHasAllColors( subCube, orangeCornerColors[ index ], 3 ) )
				break;
		
		wxASSERT( index != 4 );
		cornerQuad[ corner ] = index;
	}

	if( !TriCycleSolver::AreQuadsInSameOrder( cornerQuad, edgeQuad ) )
	{
		// I must admit that I have no proof that such a sequence will always be found.
		TriCycleSolver::TriCycleSequence triCycleSequence;
		bool found = TriCycleSolver::FindSmallestTriCycleSequenceThatOrdersQuadTheSame( cornerQuad, edgeQuad, triCycleSequence );
		wxASSERT( found );

		for( TriCycleSolver::TriCycleSequence::iterator iter = triCycleSequence.begin(); iter != triCycleSequence.end(); iter++ )
		{
			TriCycleSolver::TriCycle triCycle = *iter;

			RubiksCube::RelativeRotationSequence relativeRotationSequence;
			RubiksCube::Perspective perspective;

			switch( triCycle.direction )
			{
				case TriCycleSolver::TriCycle::FORWARD:
				{
					RubiksCube::ParseRelativeRotationSequenceString( "Li, U, R, Ui, L, U, Ri, Ui", relativeRotationSequence );
					perspective = standardPerspectivesNegated[ triCycle.invariantIndex ];
					break;
				}
				case TriCycleSolver::TriCycle::BACKWARD:
				{
					RubiksCube::ParseRelativeRotationSequenceString( "U, R, Ui, Li, U, Ri, Ui, L", relativeRotationSequence );
					perspective = standardPerspectivesNegated[ triCycle.invariantIndex ];
					break;
				}
			}

			if( relativeRotationSequence.size() > 0 )
				rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, rotationSequence );
		}
	}
}

//==================================================================================================
void SolverForCase3::PerformOrangeCornerOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	// Collect a list of all the improperly-oriented corners.
	std::list< int > improperlyOrientedCornerList;
	int corner;
	for( corner = 0; corner < 4; corner++ )
	{
		int* location = orangeCornerTargetLocations[ corner ];
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( location[0], location[1], 0 );
		if( subCube->faceColor[ RubiksCube::NEG_Z ] != RubiksCube::ORANGE )
			improperlyOrientedCornerList.push_back( corner );
	}

	if( improperlyOrientedCornerList.size() == 0 )
		return;

	corner = improperlyOrientedCornerList.front();
	RubiksCube::Perspective perspective = standardPerspectivesNegated[ corner ];
	
	int* location = orangeCornerTargetLocations[ corner ];
	c3ga::vectorE3GA perspectiveCornerVec( c3ga::vectorE3GA::coord_e1_e2_e3, double( location[0] - 1 ), double( location[1] - 1 ), 0.0 );
	perspectiveCornerVec = c3ga::unit( perspectiveCornerVec );

	RubiksCube::RelativeRotationSequence relativeRotationSequence;
	RubiksCube::ParseRelativeRotationSequenceString( "Ri, Di, R, D, Ri, Di, R, D", relativeRotationSequence );

	RubiksCube::RelativeRotationSequence relativeRotationSequenceInverse;
	RubiksCube::ParseRelativeRotationSequenceString( "Di, Ri, D, R, Di, Ri, D, R", relativeRotationSequenceInverse );

	while( improperlyOrientedCornerList.size() > 0 )
	{
		std::list< int >::iterator iter = improperlyOrientedCornerList.begin();
		corner = *iter;
		improperlyOrientedCornerList.erase( iter );
		int* location = orangeCornerTargetLocations[ corner ];
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( location[0], location[1], 0 );
		c3ga::vectorE3GA cornerVec( c3ga::vectorE3GA::coord_e1_e2_e3, double( location[0] - 1 ), double( location[1] - 1 ), 0.0 );
		cornerVec = c3ga::unit( cornerVec );

		RubiksCube::Rotation rotation;
		rotation.plane.axis = RubiksCube::Z_AXIS;
		rotation.plane.index = 0;
		rotation.angle = CalculateRotationAngle( cornerVec, perspectiveCornerVec, zAxis );
		if( rotation.angle != 0.0 )
		{
			rotationSequence.push_back( rotation );
			c3ga::bivectorE3GA blade = xAxis ^ yAxis;
			c3ga::rotorE3GA rotor = c3ga::exp( blade * 0.5 * rotation.angle );
			perspectiveCornerVec = c3ga::applyUnitVersor( rotor, perspectiveCornerVec );
		}

		RubiksCube::Face rightFace = RubiksCube::TranslateNormal( standardPerspectivesNegated[ corner ].rAxis );
		RubiksCube::Face forwardFace = RubiksCube::TranslateNormal( standardPerspectivesNegated[ corner ].fAxis );

		if( subCube->faceColor[ rightFace ] == RubiksCube::ORANGE )
			rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, rotationSequence );
		else if( subCube->faceColor[ forwardFace ] == RubiksCube::ORANGE )
			rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequenceInverse, rotationSequence );
		else
			wxASSERT( false );
	}
}

//==================================================================================================
// At this point, the relative positions of all sub-cubes in the Z=0 plane are correct,
// as well as their orientations.  Here we get the actuals positions of these sub-cubes
// correct; and to do so, we need only rotation one of them into the correct position, as
// the rest will follow, being relatively positioned as they should be.
void SolverForCase3::PerformOrangeCrossAndCornersPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	int edge;
	for( edge = 0; edge < 4; edge++ )
	{
		int* location = orangeEdgeTargetLocations[ edge ];
		const RubiksCube::SubCube* subCube = rubiksCube->Matrix( location[0], location[1], 0 );
		if( RubiksCube::CubeHasAllColors( subCube, orangeEdgeColors[0], 2 ) )
			break;
	}
	wxASSERT( edge < 4 );
	
	int* targetLocation = orangeEdgeTargetLocations[0];
	c3ga::vectorE3GA targetVec( c3ga::vectorE3GA::coord_e1_e2_e3, double( targetLocation[0] - 1 ), double( targetLocation[1] - 1 ), 0.0 );

	int* location = orangeEdgeTargetLocations[ edge ];
	c3ga::vectorE3GA vec( c3ga::vectorE3GA::coord_e1_e2_e3, double( location[0] - 1 ), double( location[1] - 1 ), 0.0 );

	RubiksCube::Rotation rotation;
	rotation.plane.axis = RubiksCube::Z_AXIS;
	rotation.plane.index = 0;
	rotation.angle = CalculateRotationAngle( vec, targetVec, zAxis );
	if( rotation.angle != 0.0 )
		rotationSequence.push_back( rotation );
}

//==================================================================================================
// TODO: This routine needs to be used throughout the stage routines.  For now, it's only being used in a few places.
// We assume here that the given vectors are of unit-length.
/*static*/ double SolverForCase3::CalculateRotationAngle( const c3ga::vectorE3GA& unitVec0, const c3ga::vectorE3GA& unitVec1, const c3ga::vectorE3GA& axis, double epsilon /*= 1e-7*/ )
{
	double angle = 0.0;
	double dot = c3ga::lc( unitVec0, unitVec1 );
	if( fabs( dot + 1.0 ) < epsilon )
		angle = M_PI;
	else if( fabs( dot ) < epsilon )
	{
		c3ga::trivectorE3GA trivector = unitVec0 ^ unitVec1 ^ axis;
		angle = M_PI / 2.0;
		if( trivector.get_e1_e2_e3() < 0.0 )
			angle = -angle;
	}
	return angle;
}

// SolverForCase3.cpp