// SolverForCase4.cpp

#include "Header.h"

//==================================================================================================
SolverForCase4::SolverForCase4( void )
{
}

//==================================================================================================
/*virtual*/ SolverForCase4::~SolverForCase4( void )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCase4::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	if( rubiksCube->SubCubeMatrixSize() != 4 )
		return false;

	FacePairList facePairList;
	MakeFacePairList( *rubiksCube, facePairList );

	if( !AreFacesSolved( *rubiksCube, facePairList ) )
	{
		if( !AreFacePairsSolved( *rubiksCube, facePairList ) )
			return SolveFacePairs( *rubiksCube, facePairList, rotationSequence );
		else
			return SolveFaces( *rubiksCube, rotationSequence );
	}
	else if( !AreEdgesSolved( *rubiksCube ) )
		return SolveEdges( *rubiksCube, rotationSequence );
	else
		return SolveAs3x3x3( *rubiksCube, rotationSequence );

	return true;
}

//==================================================================================================
/*static*/ bool SolverForCase4::AreFacesSolved( const RubiksCube& rubiksCube, const FacePairList& facePairList )
{
	FacePairList facePairsForFace;
	for( int face = 0; face < RubiksCube::CUBE_FACE_COUNT; face++ )
	{
		CollectFacePairsForFace( ( RubiksCube::Face )face, facePairList, facePairsForFace );
		if( facePairsForFace.size() != 4 )
			return false;
	}
	return true;
}

//==================================================================================================
/*static*/ bool SolverForCase4::AreFacePairsSolved( const RubiksCube& rubiksCube, const FacePairList& facePairList )
{
	FacePairList facePairsForFace;
	for( int face = 0; face < RubiksCube::CUBE_FACE_COUNT; face++ )
	{
		CollectFacePairsForFace( ( RubiksCube::Face )face, facePairList, facePairsForFace );
		if( facePairsForFace.size() == 1 )
			return false;
		if( facePairsForFace.size() == 2 )
		{
			FacePairList::iterator iter = facePairsForFace.begin();
			FacePair facePair0 = *iter++;
			FacePair facePair1 = *iter;
			if( FacePairsOverlap( facePair0, facePair1 ) )
				return false;
		}	
	}
	return true;
}

//==================================================================================================
/*static*/ bool SolverForCase4::AreEdgesSolved( const RubiksCube& rubiksCube )
{
	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::SolveFaces( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
// Find a move that will create a new face-pair without destroying an old one.
/*static*/ bool SolverForCase4::SolveFacePairs( const RubiksCube& rubiksCube, const FacePairList& facePairList, RubiksCube::RotationSequence& rotationSequence )
{
	/*const RubiksCube::SubCube* subCube0 = 0;
	const RubiksCube::SubCube* subCube1 = 0;

	// might as well return entire list of potential pairing and then try them all.  try only those that are adjacent first.  if none, make moves that make'em adjacent.
	// an algo that makes this list should be smart enough to recognize loner faces that are part of face-pairs when that face-pair overlaps another.
	bool found = FindSingleFaceSubCubesForPairing( subCube0, subCube1 );
	wxASSERT( found );

	RubiksCube::ExposedFaces( subCube0, &face0 );
	RubiksCube::ExposedFaces( subCube1, &face1 );

	if the faces are the same or opposite, do a move that gets the faces adjacent.

	RubiksCube::Perspective perspective;
	//...

	RubiksCube::Rotation setupRotation, pairingRotation;*/
	//...

	// if subCube0 is in right face and subCube1 is in up face...
	// Try moving subCube0 to pair it with subCube1.  (If that doesn't work, try moving subCube1 to pair with subCube0.)
	// apply setup rotation to a copy of the cube.
	// does pairingRotation split the face-pair paired with subCube0?  (note that you need to ignore a face-pair if it contains the loner face.)
	// If so, can a rotation be performed in that face-pair's plane that puts an unsolved face-pair location where the split will happen?  If so, do that rotation with preparation to preserve pairs.
	// does pairingRotation split the face-pair paired with subCube1?  (note that you need to ignore a face-pair if it contains the loner face.)
	// If so, can a rotation be performed in that face-pair's plane that puts an unsolved face-pair location where the split will happen?  If so, do that rotation with preparation to preserve pairs.
	// Finally, prepare other faces for pairingRotation so that no existing face-pair is split, then apply the pairing rotation.

	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::SolveEdges( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::SolveAs3x3x3( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
/*static*/ void SolverForCase4::MakeFacePairList( const RubiksCube& rubiksCube, FacePairList& facePairList )
{
	facePairList.clear();

	RubiksCube::Plane facePairPlanes[6] =
	{
		{ RubiksCube::X_AXIS, 1 },
		{ RubiksCube::X_AXIS, 2 },
		{ RubiksCube::Y_AXIS, 1 },
		{ RubiksCube::Y_AXIS, 2 },
		{ RubiksCube::Z_AXIS, 1 },
		{ RubiksCube::Z_AXIS, 2 },
	};

	FacePair facePair;
	for( int face = 0; face < RubiksCube::CUBE_FACE_COUNT; face++ )
	{
		facePair.face = ( RubiksCube::Face )face;
		for( int planeIndex = 0; planeIndex < 6; planeIndex++ )
		{
			facePair.plane = facePairPlanes[ planeIndex ];
			RubiksCube::Color colors[2];
			FacePairColors( rubiksCube, facePair, colors );
			if( colors[0] == colors[1] )
			{
				facePair.color = colors[0];
				facePairList.push_back( facePair );
			}
		}
	}
}

//==================================================================================================
/*static*/ bool SolverForCase4::FacePairColors( const RubiksCube& rubiksCube, const FacePair& facePair, RubiksCube::Color* colors )
{
	const RubiksCube::SubCube* subCube0 = 0;
	const RubiksCube::SubCube* subCube1 = 0;

	switch( facePair.face )
	{
		case RubiksCube::NEG_X:
		case RubiksCube::POS_X:
		{
			int x = ( facePair.face == RubiksCube::NEG_X ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::Y_AXIS )
			{
				subCube0 = rubiksCube.Matrix( x, facePair.plane.index, 1 );
				subCube1 = rubiksCube.Matrix( x, facePair.plane.index, 2 );
			}
			else if( facePair.plane.axis == RubiksCube::Z_AXIS )
			{
				subCube0 = rubiksCube.Matrix( x, 1, facePair.plane.index );
				subCube1 = rubiksCube.Matrix( x, 2, facePair.plane.index );
			}
			break;
		}
		case RubiksCube::NEG_Y:
		case RubiksCube::POS_Y:
		{
			int y = ( facePair.face == RubiksCube::NEG_Y ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::X_AXIS )
			{
				subCube0 = rubiksCube.Matrix( facePair.plane.index, y, 1 );
				subCube1 = rubiksCube.Matrix( facePair.plane.index, y, 2 );
			}
			else if( facePair.plane.axis == RubiksCube::Z_AXIS )
			{
				subCube0 = rubiksCube.Matrix( 1, y, facePair.plane.index );
				subCube1 = rubiksCube.Matrix( 2, y, facePair.plane.index );
			}
			break;
		}
		case RubiksCube::NEG_Z:
		case RubiksCube::POS_Z:
		{
			int z = ( facePair.face == RubiksCube::NEG_Z ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::X_AXIS )
			{
				subCube0 = rubiksCube.Matrix( facePair.plane.index, 1, z );
				subCube1 = rubiksCube.Matrix( facePair.plane.index, 2, z );
			}
			else if( facePair.plane.axis == RubiksCube::Y_AXIS )
			{
				subCube0 = rubiksCube.Matrix( 1, facePair.plane.index, z );
				subCube1 = rubiksCube.Matrix( 2, facePair.plane.index, z );
			}
			break;
		}
	}

	if( subCube0 && subCube1 )
	{
		colors[0] = subCube0->faceData[ facePair.face ].color;
		colors[1] = subCube1->faceData[ facePair.face ].color;
		return true;
	}

	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::FacePairsOverlap( const FacePair& facePair0, const FacePair& facePair1 )
{
	wxASSERT( &facePair0 != &facePair1 );

	if( facePair0.face != facePair1.face )
		return false;

	if( facePair0.plane.axis == facePair1.plane.axis )
		return false;

	return true;
}

//==================================================================================================
/*static*/ void SolverForCase4::RotationThatPreservesFacePairs( const RubiksCube& rubiksCube, RubiksCube::Face face, const FacePairList& facePairList,
																	const RubiksCube::Rotation& anticipatedRotation, RubiksCube::Rotation& rotation )
{
	rotation.angle = 0.0;
	rotation.plane = rubiksCube.TranslateFace( face );

	FacePairList facePairsForFace;
	CollectFacePairsForFace( face, facePairList, facePairsForFace );

	if( facePairsForFace.size() == 4 )
		return;

	if( facePairsForFace.size() == 2 )
	{
		FacePairList::iterator iter = facePairsForFace.begin();
		FacePair facePair0 = *iter++;
		FacePair facePair1 = *iter;
		if( FacePairsOverlap( facePair0, facePair1 ) || !AnticipatedRotationSplitsPair( anticipatedRotation, facePair0 ) )
			return;
	}

	if( facePairsForFace.size() == 1 )
	{
		FacePairList::iterator iter = facePairsForFace.begin();
		FacePair facePair = *iter;
		if( !AnticipatedRotationSplitsPair( anticipatedRotation, facePair ) )
			return;
	}

	rotation.angle = M_PI / 2.0;
}

//==================================================================================================
/*static*/ bool SolverForCase4::AnticipatedRotationSplitsPair( const RubiksCube::Rotation& anticipatedRotation, const FacePair& facePair )
{
	if( anticipatedRotation.plane.index == 0 || anticipatedRotation.plane.index == 3 )
		return false;

	if( facePair.plane.axis == facePair.plane.axis )
		return false;

	return true;
}

//==================================================================================================
/*static*/ void SolverForCase4::CollectFacePairsForFace( RubiksCube::Face face, const FacePairList& facePairList, FacePairList& facePairListForFace )
{
	facePairListForFace.clear();
	for( FacePairList::const_iterator iter = facePairList.begin(); iter != facePairList.end(); iter++ )
	{
		FacePair facePair = *iter;
		if( facePair.face == face )
			facePairListForFace.push_back( facePair );
	}
}

// SolverForCase4.cpp