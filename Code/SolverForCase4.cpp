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
	else if( !AreEdgePairsSolved( *rubiksCube ) )
		return SolveEdgePairs( *rubiksCube, rotationSequence );
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
/*static*/ bool SolverForCase4::SolveFaces( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
// Find a rotation sequence that will create a new face-pair without destroying an old one.
/*static*/ bool SolverForCase4::SolveFacePairs( const RubiksCube& rubiksCube, const FacePairList& facePairList, RubiksCube::RotationSequence& rotationSequence )
{
	PotentialFacePairList potentialFacePairList;
	FindAllPotentialFacePairings( rubiksCube, facePairList, potentialFacePairList );
	if( potentialFacePairList.size() == 0 )
		return false;

	// When a pair of overlaping (solved) face-pairs exists, one of those pairs can be split without worry in
	// order to solve another face-pair.  But if this in turn creates a different instance of the same situation,
	// then the computer may choose to solve it in a way that resurrects the original instance of this situation.
	// And by this it may happen that our algorithm loops indefinitely.  Although an unlikely case, to avoid it,
	// we should shuffle the the potential face-pair list before iterating over it.
	// TODO: Put shuffle here.

	for( PotentialFacePairList::iterator iter = potentialFacePairList.begin(); iter != potentialFacePairList.end(); iter++ )
	{
		PotentialFacePair potentialFacePair = *iter;
		SolveFacePairing( rubiksCube, facePairList, potentialFacePair, rotationSequence );
		if( rotationSequence.size() > 0 )
			return true;
	}

	// If we were unable to solve any potential face-pairings, go try to prepare a potential face-pairing for success on a subsequent attempt.
	for( PotentialFacePairList::iterator iter = potentialFacePairList.begin(); iter != potentialFacePairList.end(); iter++ )
	{
		PotentialFacePair potentialFacePair = *iter;
		PrepareFacePairing( rubiksCube, facePairList, potentialFacePair, rotationSequence );
		if( rotationSequence.size() > 0 )
			return true;
	}

	// There is a parity problem that arises 50% of the time in solving the last two pairs.
	// To fix it, what you have to do is find an existing pair by the same color as one of the
	// last two pairs you're trying to match, and then move that pair into the right face, then
	// move out the other pair that it makes without destroying the adjacency of the existing
	// pair-match situation.  We have to be careful, though, because we might not end up with un-related
	// colors in the up and right faces, which is what we want, so we have to make sure that happens.
	// If we put it in, do a quarter turn, then move it the reverse way back out, that should always happen,
	// because we started with two unrelated colors paired in the up and right faces.
	if( potentialFacePairList.size() == 2 )
	{
		//...

		// Look at just the first of the two potential face pairings.
		// Find the completed face pair for its color.
		// Move it into the right face so that it occupies that face with *both* potentially pair-able face cubes from both unresolved pairing problems.  (they will share a face.)
		// Now do the right quater turn, then move out the other completed pair by the same distance, but opposite rotation direction.
		// At this point, the algorithm should be able to resume without the parity problem.
	}

	return false;
}

//==================================================================================================
// Here we are always trying to move subCube0 from the right-face into the up-face to be paired with the subCube1.
// Again, it's apparent from this code that if we were trying to solve a higher-order cube, this would just be too slow.
/*static*/ void SolverForCase4::SolveFacePairing( const RubiksCube& rubiksCube, const FacePairList& facePairList,
													const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence )
{
	if( !RubiksCube::AreAdjacentFaces( potentialFacePair.face[0], potentialFacePair.face[1] ) )
		return;

	// TODO: A more sophisticated version of this algorithm would consider multiple setup and pairing rotations.
	RubiksCube::Rotation setupRotation, pairingRotation;
	
	// TODO: Determine setup rotation and pairingRotation here...

	SituationStack situationStack( &rubiksCube );
	situationStack.Push( setupRotation );

	FacePair potentialSplitPair[2];

	potentialSplitPair[0].plane = situationStack.Top()->rubiksCube->TranslateFace( RubiksCube::TranslateNormal( potentialFacePair.perspective.uAxis ) );
	potentialSplitPair[0].face = potentialFacePair.face[0];
	//potentialSplitPair[0].plane.index = 3 - situationStack.Top()->rubiksCube->FindPlaneContainingSubCube( potentialSplitPair[0].plane.axis, potentialFacePair.subCube[0] );

	potentialSplitPair[1].plane = situationStack.Top()->rubiksCube->TranslateFace( RubiksCube::TranslateNormal( potentialFacePair.perspective.rAxis ) );
	potentialSplitPair[1].face = potentialFacePair.face[1];
	//potentialSplitPair[1].plane.index = 3 - situationStack.Top()->rubiksCube->FindPlaneContainingSubCube( potentialSplitPair[1].plane.axis, potentialFacePair.subCube[1] );
	
	for( int pair = 0; pair < 2 && rotationSequence.size() == 0; pair++ )
	{
		int otherPair = ( pair + 1 ) % 2;

		RubiksCube::RotationSequence firstFacePairPreservationSequence;
		if( !FindFacePairPreservationSequence( situationStack, potentialSplitPair[ pair ], pairingRotation, potentialFacePair.perspective, firstFacePairPreservationSequence ) )
			continue;
		
		if( firstFacePairPreservationSequence.size() > 0 )
		{
			situationStack.Push( firstFacePairPreservationSequence );
			
			// It's important to note that the second preservation sequence should never undo the preservation of the first such sequence.
			RubiksCube::RotationSequence secondFacePairPreservationSequence;
			if( !FindFacePairPreservationSequence( situationStack, potentialSplitPair[ otherPair ], pairingRotation, potentialFacePair.perspective, secondFacePairPreservationSequence ) )
			{
				situationStack.Pop();
				continue;
			}

			if( secondFacePairPreservationSequence.size() > 0 )
				situationStack.Push( secondFacePairPreservationSequence );
		}
		else
		{
			if( !FindFacePairPreservationSequence( situationStack, potentialSplitPair[ otherPair ], pairingRotation, potentialFacePair.perspective, firstFacePairPreservationSequence ) )
				continue;

			if( firstFacePairPreservationSequence.size() > 0 )
				situationStack.Push( firstFacePairPreservationSequence );
		}
		
		// If we get here, the pairing is solved!  Formulate the solution.
		// Note that any zero-angle rotations we append here will just get optimized out.
		rotationSequence.push_back( setupRotation );
		rotationSequence.insert( rotationSequence.end(), situationStack.Top()->rotationSequence.begin(), situationStack.Top()->rotationSequence.end() );
		FacePairList facePairList;
		MakeFacePairList( *situationStack.Top()->rubiksCube, facePairList );
		RubiksCube::Rotation rotation;
		RotationThatPreservesFacePairs( *situationStack.Top()->rubiksCube, RubiksCube::TranslateNormal( -potentialFacePair.perspective.uAxis ), facePairList, pairingRotation, rotation );
		rotationSequence.push_back( rotation );
		RotationThatPreservesFacePairs( *situationStack.Top()->rubiksCube, RubiksCube::TranslateNormal( -potentialFacePair.perspective.rAxis ), facePairList, pairingRotation, rotation );
		rotationSequence.push_back( rotation );
		rotationSequence.push_back( pairingRotation );
	}
}

//==================================================================================================
// The name of this method is not really specific enough.  There are multiple ways to preserve
// a face-pair from an impending rotation, but here we're trying to find a specific way of preserving
// a potentially split face-pair by rotating in the given potentially split face-pair's plane.
/*static*/ bool SolverForCase4::FindFacePairPreservationSequence( SituationStack& situationStack, const FacePair& potentialSplitPair,
																	const RubiksCube::Rotation& anticipatedRotation, const RubiksCube::Perspective& perspective,
																	RubiksCube::RotationSequence& preservationSequence )
{
	preservationSequence.clear();

	// If there is no pairing to be split, then the empty sequence is all we need.
	if( !IsFacePairValid( *situationStack.Top()->rubiksCube, potentialSplitPair ) )
		return true;

	FacePairList facePairList;
	MakeFacePairList( *situationStack.Top()->rubiksCube, facePairList );

	// This appears to be brute force, but it seems no different to me than looking at a case-by-case basis anyway.
	for( int quaterTurnCount = 1; quaterTurnCount < 4 && preservationSequence.size() == 0; quaterTurnCount++ )
	{
		RubiksCube::Rotation avoidanceRotation;
		avoidanceRotation.plane = potentialSplitPair.plane;
		avoidanceRotation.angle = double( quaterTurnCount ) * M_PI / 2.0;
		
		for( int orientationPermutation = 0; orientationPermutation < 8; orientationPermutation++ )
		{
			RubiksCube::RotationSequence trialSequence;
			RubiksCube::Rotation rotation;
			
			RotationThatPreservesFacePairs( *situationStack.Top()->rubiksCube, RubiksCube::TranslateNormal( perspective.fAxis ), facePairList, avoidanceRotation, rotation );
			if( orientationPermutation & 0x1 )
				rotation.angle += M_PI;
			trialSequence.push_back( rotation );

			RotationThatPreservesFacePairs( *situationStack.Top()->rubiksCube, RubiksCube::TranslateNormal( -perspective.fAxis ), facePairList, avoidanceRotation, rotation );
			if( orientationPermutation & 0x2 )
				rotation.angle += M_PI;
			trialSequence.push_back( rotation );

			if( potentialSplitPair.face == RubiksCube::TranslateNormal( perspective.uAxis ) )
			{
				RotationThatPreservesFacePairs( *situationStack.Top()->rubiksCube, RubiksCube::TranslateNormal( -perspective.uAxis ), facePairList, avoidanceRotation, rotation );
				if( orientationPermutation & 0x4 )
					rotation.angle += M_PI;
				trialSequence.push_back( rotation );
			}
			else if( potentialSplitPair.face == RubiksCube::TranslateNormal( perspective.rAxis ) )
			{
				RotationThatPreservesFacePairs( *situationStack.Top()->rubiksCube, RubiksCube::TranslateNormal( -perspective.rAxis ), facePairList, avoidanceRotation, rotation );
				if( orientationPermutation & 0x4 )
					rotation.angle += M_PI;
				trialSequence.push_back( rotation );
			}
		
			trialSequence.push_back( avoidanceRotation );
			situationStack.Push( trialSequence );

			if( !IsFacePairValid( *situationStack.Top()->rubiksCube, potentialSplitPair ) )
				preservationSequence = trialSequence;

			situationStack.Pop();
		}
	}

	return ( preservationSequence.size() == 0 ) ? false : true;
}

//==================================================================================================
// Here we take the two cases of the potential face-pairs of being either on opposite sides of
// the cube or on the same side, and return a sequence of moves that gets them on adjacent sides
// of the cube.  The potential face-pair solver can then deal with that case.
/*static*/ void SolverForCase4::PrepareFacePairing( const RubiksCube& rubiksCube, const FacePairList& facePairList, const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence )
{
	if( RubiksCube::AreOppositeFaces( potentialFacePair.face[0], potentialFacePair.face[1] ) )
	{
	}
	else if( potentialFacePair.face[0] == potentialFacePair.face[1] )
	{
	}
}

//==================================================================================================
// If we were trying to solve a higher-order cube, a routine like this would be a major bottle-neck,
// and we would need to cache our book-keeping and use more efficient data-structures and algorithms.
/*static*/ void SolverForCase4::FindAllPotentialFacePairings( const RubiksCube& rubiksCube, const FacePairList& facePairList, PotentialFacePairList& potentialFacePairList )
{
	RubiksCube::SubCubeVector subCubeVector;
	
	for( int color = RubiksCube::YELLOW; color <= RubiksCube::ORANGE; color++ )
	{
		// Go collect all face cubes of a certain color.
		subCubeVector.clear();
		rubiksCube.CollectSubCubes( ( RubiksCube::Color* )&color, 1, subCubeVector );
		wxASSERT( subCubeVector.size() == 4 );

		// Now go see which pairs of these face cubes are potentially pairable.
		// It's important to realize here that we are creating redundant pairings.
		// (i.e., if we list pair (a,b), we also list pair (b,a).)  What this does
		// is simplify the solving code, because it allows the solving code to consider
		// the problem from just one perspective.  If we only listed (a,b) and not (b,a),
		// we would have to consider (a,b) from potentially two different perspectives
		// before being able to solve it.
		for( int i = 0; i < 4; i++ )
		{
			const RubiksCube::SubCube* subCube0 = subCubeVector[i];
			if( !IsSolitaryFaceCube( rubiksCube, subCube0, facePairList ) )
				continue;

			for( int j = 0; j < 4; j++ )
			{
				if( i == j )
					continue;

				const RubiksCube::SubCube* subCube1 = subCubeVector[j];
				if( !IsSolitaryFaceCube( rubiksCube, subCube1, facePairList ) )
					continue;

				PotentialFacePair potentialFacePair;
				potentialFacePair.subCube[0] = subCube0;
				potentialFacePair.subCube[1] = subCube1;
				RubiksCube::ExposedFaces( subCube0, &potentialFacePair.face[0] );
				RubiksCube::ExposedFaces( subCube1, &potentialFacePair.face[1] );

				c3ga::trivectorE3GA I( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 );

				if( RubiksCube::AreAdjacentFaces( potentialFacePair.face[0], potentialFacePair.face[1] ) )
				{
					potentialFacePair.perspective.rAxis = RubiksCube::TranslateNormal( potentialFacePair.face[0] );
					potentialFacePair.perspective.uAxis = RubiksCube::TranslateNormal( potentialFacePair.face[1] );
				}
				else
				{
					potentialFacePair.perspective.uAxis = RubiksCube::TranslateNormal( potentialFacePair.face[1] );
					potentialFacePair.perspective.rAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3,
																potentialFacePair.perspective.uAxis.get_e2(),
																potentialFacePair.perspective.uAxis.get_e3(),
																potentialFacePair.perspective.uAxis.get_e1() );	// Arbitrarily choose an axis orthogonal to the up-axis.
				}

				// Make a right-handed system for the perspective.  (It must always be so.)
				potentialFacePair.perspective.fAxis = c3ga::gp( potentialFacePair.perspective.rAxis ^ potentialFacePair.perspective.uAxis, -I );

				potentialFacePairList.push_back( potentialFacePair );
			}
		}
	}
}

//==================================================================================================
/*static*/ bool SolverForCase4::IsSolitaryFaceCube( const RubiksCube& rubiksCube, const RubiksCube::SubCube* subCube, const FacePairList& facePairList )
{
	FacePairList facePairListForSubCube;
	CollectFacePairsForSubCube( rubiksCube, subCube, facePairList, facePairListForSubCube );
	int count = facePairListForSubCube.size();
	wxASSERT( 0 <= count && count <= 2 );
	if( count == 0 )
		return true;
	if( count == 2 )
		return false;

	// If the count is 1, then the sub-cube is solitary if the face-pair of which
	// it is a member overlaps some other face-pair.
	for( FacePairList::const_iterator iter = facePairList.begin(); iter != facePairList.end(); iter++ )
	{
		FacePair facePair = *iter;
		if( FacePairsOverlap( facePair, facePairList.front() ) )
			return true;
	}

	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::AreEdgePairsSolved( const RubiksCube& rubiksCube )
{
	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::SolveEdgePairs( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence )
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
			if( IsFacePairValid( rubiksCube, facePair ) )
				facePairList.push_back( facePair );
		}
	}
}

//==================================================================================================
/*static*/ bool SolverForCase4::IsFacePairValid( const RubiksCube& rubiksCube, const FacePair& facePair )
{
	RubiksCube::Color colors[2];
	if( !FacePairColors( rubiksCube, facePair, colors ) )
		return false;
	if( colors[0] != colors[1] )
		return false;
	return true;
}

//==================================================================================================
/*static*/ bool SolverForCase4::FacePairColors( const RubiksCube& rubiksCube, const FacePair& facePair, RubiksCube::Color* colors )
{
	const RubiksCube::SubCube* subCubes[2];
	if( !FacePairSubCubes( rubiksCube, facePair, subCubes ) )
		return false;

	colors[0] = subCubes[0]->faceData[ facePair.face ].color;
	colors[1] = subCubes[1]->faceData[ facePair.face ].color;
	return true;
}

//==================================================================================================
/*static*/ bool SolverForCase4::FacePairSubCubes( const RubiksCube& rubiksCube, const FacePair& facePair, const RubiksCube::SubCube** subCubes )
{
	switch( facePair.face )
	{
		case RubiksCube::NEG_X:
		case RubiksCube::POS_X:
		{
			int x = ( facePair.face == RubiksCube::NEG_X ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::Y_AXIS )
			{
				subCubes[0] = rubiksCube.Matrix( x, facePair.plane.index, 1 );
				subCubes[1] = rubiksCube.Matrix( x, facePair.plane.index, 2 );
			}
			else if( facePair.plane.axis == RubiksCube::Z_AXIS )
			{
				subCubes[0] = rubiksCube.Matrix( x, 1, facePair.plane.index );
				subCubes[1] = rubiksCube.Matrix( x, 2, facePair.plane.index );
			}
			break;
		}
		case RubiksCube::NEG_Y:
		case RubiksCube::POS_Y:
		{
			int y = ( facePair.face == RubiksCube::NEG_Y ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::X_AXIS )
			{
				subCubes[0] = rubiksCube.Matrix( facePair.plane.index, y, 1 );
				subCubes[1] = rubiksCube.Matrix( facePair.plane.index, y, 2 );
			}
			else if( facePair.plane.axis == RubiksCube::Z_AXIS )
			{
				subCubes[0] = rubiksCube.Matrix( 1, y, facePair.plane.index );
				subCubes[1] = rubiksCube.Matrix( 2, y, facePair.plane.index );
			}
			break;
		}
		case RubiksCube::NEG_Z:
		case RubiksCube::POS_Z:
		{
			int z = ( facePair.face == RubiksCube::NEG_Z ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::X_AXIS )
			{
				subCubes[0] = rubiksCube.Matrix( facePair.plane.index, 1, z );
				subCubes[1] = rubiksCube.Matrix( facePair.plane.index, 2, z );
			}
			else if( facePair.plane.axis == RubiksCube::Y_AXIS )
			{
				subCubes[0] = rubiksCube.Matrix( 1, facePair.plane.index, z );
				subCubes[1] = rubiksCube.Matrix( 2, facePair.plane.index, z );
			}
			break;
		}
	}

	if( subCubes[0] && subCubes[1] )
		return true;

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
// Notice that we don't check if the given rotation is a zero-angle rotation.
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

//==================================================================================================
/*static*/ bool SolverForCase4::CollectFacePairForFaceAndPlane( RubiksCube::Face face, const RubiksCube::Plane& plane, const FacePairList& facePairList, FacePair& facePair )
{
	FacePairList facePairListForFace;
	CollectFacePairsForFace( face, facePairList, facePairListForFace );

	FacePairList facePairListForFaceAndPlane;
	for( FacePairList::iterator iter = facePairListForFace.begin(); iter != facePairListForFace.end(); iter++ )
	{
		facePair = *iter;
		if( facePair.plane.axis == plane.axis && facePair.plane.index == plane.index )
			facePairListForFaceAndPlane.push_back( facePair );
	}

	wxASSERT( facePairListForFaceAndPlane.size() == 0 || facePairListForFaceAndPlane.size() == 1 );
	if( facePairListForFaceAndPlane.size() == 0 )
		return false;

	facePair = facePairListForFaceAndPlane.front();
	return true;
}

//==================================================================================================
/*static*/ void SolverForCase4::CollectFacePairsForSubCube( const RubiksCube& rubiksCube, const RubiksCube::SubCube* subCube, const FacePairList& facePairList, FacePairList& facePairListForSubCube )
{
	facePairListForSubCube.clear();
	for( FacePairList::const_iterator iter = facePairList.begin(); iter != facePairList.end(); iter++ )
	{
		FacePair facePair = *iter;
		const RubiksCube::SubCube* subCubes[2];
		FacePairSubCubes( rubiksCube, facePair, subCubes );
		if( subCubes[0] == subCube || subCubes[1] == subCube )
			facePairListForSubCube.push_back( facePair );
	}
}

// SolverForCase4.cpp