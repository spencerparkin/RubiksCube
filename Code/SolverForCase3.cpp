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
	// Find the Red/Yellow edge peice.
	// If it is not already in position.
	//   If it is in the Z=2 plane, rotate it to the Z=0 plane, return.
	//   If it is in the Z=0 plane, rotate it in that plane to the right spot.
	//   If it was already in the right spot
}

//==================================================================================================
void SolverForCase3::PerformRedCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
}

// SolverForCase3.cpp