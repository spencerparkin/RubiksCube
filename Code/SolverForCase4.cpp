// SolverForCase4.cpp

#include "Header.h"

//==================================================================================================
SolverForCase4::SolverForCase4( void )
{
	situationStack = 0;
}

//==================================================================================================
/*virtual*/ SolverForCase4::~SolverForCase4( void )
{
}

//==================================================================================================
/*virtual*/ Solver::SituationStack::Situation* SolverForCase4::CreateSituation( void )
{
	return new Situation();
}

//==================================================================================================
/*virtual*/ void SolverForCase4::PostPush( SituationStack::Situation* situation )
{
	// We may want to disable this in the cases where we don't need it.
	MakeFacePairList( ( ( Situation* )situation )->facePairList );
}

//==================================================================================================
/*virtual*/ void SolverForCase4::PrePop( SituationStack::Situation* situation )
{
}

//==================================================================================================
/*virtual*/ bool SolverForCase4::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	bool success = true;

	if( rubiksCube->SubCubeMatrixSize() != 4 )
		success = false;
	else
	{
		situationStack = new SituationStack( this, rubiksCube );
		situationStack->Push();

		if( !AreFacesSolved() )
		{
			if( !AreFacePairsSolved() )
				success = SolveFacePairs( rotationSequence );
			else
				success = true; // Do this for now so that we can debug the face-pair solver.
				//success = SolveFaces( rotationSequence );
		}
		else if( !AreEdgePairsSolved() )
			success = SolveEdgePairs( rotationSequence );
		else
			success = SolveAs3x3x3( rotationSequence );

		delete situationStack;
		situationStack = 0;
	}

	return success;
}

//==================================================================================================
bool SolverForCase4::AreFacesSolved( void )
{
	for( int face = 0; face < RubiksCube::CUBE_FACE_COUNT; face++ )
	{
		FacePairList facePairsForFace;
		CollectFacePairsForFace( ( RubiksCube::Face )face, facePairsForFace );
		if( facePairsForFace.size() != 4 )
			return false;
	}
	return true;
}

//==================================================================================================
bool SolverForCase4::AreFacePairsSolved( void )
{
	for( int face = 0; face < RubiksCube::CUBE_FACE_COUNT; face++ )
	{
		FacePairList facePairsForFace;
		CollectFacePairsForFace( ( RubiksCube::Face )face, facePairsForFace );
		if( facePairsForFace.size() == 1 )
			return false;
		if( facePairsForFace.size() == 2 )
		{
			FacePairList::iterator iter = facePairsForFace.begin();
			FacePair facePair0 = *iter++;
			FacePair facePair1 = *iter;
			if( FacePairsOverlapAndAreDistinct( facePair0, facePair1 ) )
				return false;
		}	
	}
	return true;
}

//==================================================================================================
bool SolverForCase4::SolveFaces( RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
// Find a rotation sequence that will create a new face-pair without destroying an old one.
bool SolverForCase4::SolveFacePairs( RubiksCube::RotationSequence& rotationSequence )
{
	PotentialFacePairList potentialFacePairList;
	FindAllPotentialFacePairings( potentialFacePairList );
	if( potentialFacePairList.size() == 0 )
		return false;

	// When a pair of overlaping (solved) face-pairs exists, one of those pairs can be split without worry in
	// order to solve another face-pair.  But if this in turn creates a different instance of the same situation,
	// then the computer may choose to solve it in a way that resurrects the original instance of this situation.
	// And by this it may happen that our algorithm loops indefinitely.  Although an unlikely case, to avoid it,
	// we should shuffle the the potential face-pair list before iterating over it.  This isn't the most elegant
	// solution, but again, although unlikely, if it does happen, it's possible that our final solution sequence has
	// a bunch of redundant moves in it, but the optimizer should have no problem removing those.
	// TODO: Put shuffle here.

	for( PotentialFacePairList::iterator iter = potentialFacePairList.begin(); iter != potentialFacePairList.end(); iter++ )
	{
		PotentialFacePair potentialFacePair = *iter;
		SolveFacePairing( potentialFacePair, rotationSequence );
		if( rotationSequence.size() > 0 )
			return true;
	}

	// If we were unable to solve any potential face-pairings, go try to prepare a potential face-pairing for success on a subsequent attempt.
	for( PotentialFacePairList::iterator iter = potentialFacePairList.begin(); iter != potentialFacePairList.end(); iter++ )
	{
		PotentialFacePair potentialFacePair = *iter;
		PrepareFacePairing( potentialFacePair, rotationSequence );
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
// Here a placement number is returned based upon the relative position given.  This is used in
// the face-pairing solver logic.
/*static*/ int SolverForCase4::PlacementForFacePairingSolver( const RubiksCube::Coordinates& relativeCoords )
{
	// TODO: Combine these maps into one map.
	int rightMap[2][2] = { { 3, 0 }, { 2, 1 } };
	int upMap[2][2] = { { 2, 1 }, { 3, 0 } };
	if( relativeCoords.x == 3 )
		return rightMap[ relativeCoords.y - 1 ][ relativeCoords.z - 1 ];
	else if( relativeCoords.y == 3 )
		return upMap[ relativeCoords.x - 1 ][ relativeCoords.z - 1 ];	
	return -1;
}

//==================================================================================================
// Here we are always trying to move subCube[0] from the right-face into the up-face to be paired with subCube[1].
// Again, it's apparent from this code that if we were trying to solve a higher-order cube, this would just be too slow.
// It should also be noted that, while playing with a 4x4x4, I've noticed cases where this algorithm certainly
// does not find the optimal solution.  For example, there are cases where one rotation completes several pairs,
// but this algorithm is not capable of detecting that.  One optimization would be the ability to recognize when
// splitting an already-solved face-pairing is okay, because the move still completes more pairs.  In any case,
// correctness is more important than speed to begin with.
void SolverForCase4::SolveFacePairing( const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence )
{
	if( !RubiksCube::AreAdjacentFaces( potentialFacePair.face[0], potentialFacePair.face[1] ) )
		return;

	int situationStackSize = situationStack->Size();

	for( int quaterTurnCount = 0; quaterTurnCount < 4; quaterTurnCount++ )
	{
		// Try this orientation of the up-face.
		RubiksCube::Rotation rotation;
		rotation.angle = double( quaterTurnCount ) * M_PI / 2.0;
		rotation.plane = situationStack->Top()->rubiksCube->TranslateFace( RubiksCube::TranslateNormal( potentialFacePair.perspective.uAxis ) );
		situationStack->Push( rotation );

		// Now determine the proper orientation of the right-face so that subCube[0] is ready to be rotated next to subCube[1].
		const RubiksCube::SubCube* subCube[2];
		subCube[0] = situationStack->Top()->rubiksCube->FindSubCubeById( potentialFacePair.subCube[0]->id );
		subCube[1] = situationStack->Top()->rubiksCube->FindSubCubeById( potentialFacePair.subCube[1]->id );
		RubiksCube::Coordinates relativeCoords[2];
		situationStack->Top()->rubiksCube->RelativeFromActual( subCube[0]->coords, relativeCoords[0], potentialFacePair.perspective );
		situationStack->Top()->rubiksCube->RelativeFromActual( subCube[1]->coords, relativeCoords[1], potentialFacePair.perspective );
		int faceCubePlacement[2] = { PlacementForFacePairingSolver( relativeCoords[0] ), PlacementForFacePairingSolver( relativeCoords[1] ) };
		int targetPlacement = 3 - faceCubePlacement[1];
		int placementDelta = targetPlacement - faceCubePlacement[0];
		rotation.angle = double( placementDelta ) * M_PI / 2.0;
		rotation.plane = situationStack->Top()->rubiksCube->TranslateFace( RubiksCube::TranslateNormal( potentialFacePair.perspective.rAxis ) );
		situationStack->Push( rotation );
		
		// Determine the rotation that completes the potential pairing.
		RubiksCube::RelativeRotation relativeRotation( RubiksCube::RelativeRotation::Fi );
		subCube[0] = situationStack->Top()->rubiksCube->FindSubCubeById( potentialFacePair.subCube[0]->id );
		situationStack->Top()->rubiksCube->RelativeFromActual( subCube[0]->coords, relativeCoords[0], potentialFacePair.perspective );
		faceCubePlacement[0] = PlacementForFacePairingSolver( relativeCoords[0] );
		if( faceCubePlacement[0] == 0 || faceCubePlacement[0] == 1 )
			relativeRotation.planeIndex = 1;
		else if( faceCubePlacement[0] == 2 || faceCubePlacement[0] == 3 )
			relativeRotation.planeIndex = 2;
		else
			wxASSERT( false );
		RubiksCube::Rotation pairingRotation;
		situationStack->Top()->rubiksCube->TranslateRotation( potentialFacePair.perspective, relativeRotation, pairingRotation );

		// Now determine the face-pairs, which may be complete, (or may not be), that are potentially split by the pairing rotation.
		subCube[1] = situationStack->Top()->rubiksCube->FindSubCubeById( potentialFacePair.subCube[1]->id );
		FacePair potentialSplitPair[2];
		potentialSplitPair[0].plane = situationStack->Top()->rubiksCube->TranslateFace( RubiksCube::TranslateNormal( potentialFacePair.perspective.uAxis ) );
		potentialSplitPair[0].face = potentialFacePair.face[0];
		potentialSplitPair[0].plane.index = 3 - RubiksCube::PlaneContainingSubCube( potentialSplitPair[0].plane.axis, subCube[0] );
		potentialSplitPair[1].plane = situationStack->Top()->rubiksCube->TranslateFace( RubiksCube::TranslateNormal( potentialFacePair.perspective.rAxis ) );
		potentialSplitPair[1].face = potentialFacePair.face[1];
		potentialSplitPair[1].plane.index = 3 - RubiksCube::PlaneContainingSubCube( potentialSplitPair[1].plane.axis, subCube[1] );
		
		// Now go search for a way to make the pairing rotation without spliting up any existing and completed face pairs.
		for( int pair = 0; pair < 2; pair++ )
		{
			int otherPair = ( pair + 1 ) % 2;

			RubiksCube::RotationSequence firstFacePairPreservationSequence;
			if( !FindFacePairPreservationSequence( potentialSplitPair[ pair ], pairingRotation, potentialFacePair.perspective, firstFacePairPreservationSequence ) )
				continue;
			
			if( firstFacePairPreservationSequence.size() > 0 )
			{
				situationStack->Push( &firstFacePairPreservationSequence );
				
				// It's important to note that the second preservation sequence should never undo the preservation of the first such sequence.
				RubiksCube::RotationSequence secondFacePairPreservationSequence;
				if( !FindFacePairPreservationSequence( potentialSplitPair[ otherPair ], pairingRotation, potentialFacePair.perspective, secondFacePairPreservationSequence ) )
				{
					situationStack->Pop();
					continue;
				}

				if( secondFacePairPreservationSequence.size() > 0 )
					situationStack->Push( &secondFacePairPreservationSequence );
			}
			else
			{
				if( !FindFacePairPreservationSequence( potentialSplitPair[ otherPair ], pairingRotation, potentialFacePair.perspective, firstFacePairPreservationSequence ) )
					continue;

				if( firstFacePairPreservationSequence.size() > 0 )
					situationStack->Push( &firstFacePairPreservationSequence );
			}
			
			// If we get here, the pairing is solved!  Formulate the solution.
			// Note that any zero-angle rotations we append here will just get optimized out.
			situationStack->AppendRotationSequence( rotationSequence );
			RotationThatPreservesFacePairs( RubiksCube::TranslateNormal( -potentialFacePair.perspective.uAxis ), pairingRotation, rotation );
			rotationSequence.push_back( rotation );
			RotationThatPreservesFacePairs( RubiksCube::TranslateNormal( -potentialFacePair.perspective.rAxis ), pairingRotation, rotation );
			rotationSequence.push_back( rotation );
			rotationSequence.push_back( pairingRotation );
			return;
		}

		situationStack->Pop();
		situationStack->Pop();

		wxASSERT( situationStack->Size() == situationStackSize );
	}
}

//==================================================================================================
// The name of this method is not really specific enough.  There are multiple ways to preserve
// a face-pair from an impending rotation, but here we're trying to find a specific way of preserving
// a potentially split face-pair by rotating in the given potentially split face-pair's plane.
bool SolverForCase4::FindFacePairPreservationSequence( const FacePair& potentialSplitPair,
														const RubiksCube::Rotation& anticipatedRotation,
														const RubiksCube::Perspective& perspective,
														RubiksCube::RotationSequence& preservationSequence )
{
	preservationSequence.clear();

	// If there is no pairing to be split, then the empty sequence is all we need.
	if( !IsFacePairValid( potentialSplitPair ) )
		return true;

	// This appears to be a brute force search, but it seems no different to me than looking at a case-by-case basis anyway.
	for( int quaterTurnCount = 1; quaterTurnCount < 4 && preservationSequence.size() == 0; quaterTurnCount++ )
	{
		RubiksCube::Rotation avoidanceRotation;
		avoidanceRotation.plane = potentialSplitPair.plane;
		avoidanceRotation.angle = double( quaterTurnCount ) * M_PI / 2.0;
		
		for( int orientationPermutation = 0; orientationPermutation < 8; orientationPermutation++ )
		{
			RubiksCube::RotationSequence trialSequence;
			RubiksCube::Rotation rotation;
			
			RotationThatPreservesFacePairs( RubiksCube::TranslateNormal( perspective.fAxis ), avoidanceRotation, rotation );
			if( orientationPermutation & 0x1 )
				rotation.angle += M_PI;
			trialSequence.push_back( rotation );

			RotationThatPreservesFacePairs( RubiksCube::TranslateNormal( -perspective.fAxis ), avoidanceRotation, rotation );
			if( orientationPermutation & 0x2 )
				rotation.angle += M_PI;
			trialSequence.push_back( rotation );

			if( potentialSplitPair.face == RubiksCube::TranslateNormal( perspective.uAxis ) )
			{
				RotationThatPreservesFacePairs( RubiksCube::TranslateNormal( -perspective.uAxis ), avoidanceRotation, rotation );
				if( orientationPermutation & 0x4 )
					rotation.angle += M_PI;
				trialSequence.push_back( rotation );
			}
			else if( potentialSplitPair.face == RubiksCube::TranslateNormal( perspective.rAxis ) )
			{
				RotationThatPreservesFacePairs( RubiksCube::TranslateNormal( -perspective.rAxis ), avoidanceRotation, rotation );
				if( orientationPermutation & 0x4 )
					rotation.angle += M_PI;
				trialSequence.push_back( rotation );
			}
		
			trialSequence.push_back( avoidanceRotation );
			situationStack->Push( &trialSequence );

			if( !IsFacePairValid( potentialSplitPair ) )
				preservationSequence = trialSequence;

			situationStack->Pop();
		}
	}

	return ( preservationSequence.size() == 0 ) ? false : true;
}

//==================================================================================================
// Here we take the two cases of the potential face-pairs of being either on opposite sides of
// the cube or on the same side, and return a sequence of moves that gets them on adjacent sides
// of the cube.  The potential face-pair solver can then deal with that case.
void SolverForCase4::PrepareFacePairing( const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence )
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
void SolverForCase4::FindAllPotentialFacePairings( PotentialFacePairList& potentialFacePairList )
{
	RubiksCube::SubCubeVector subCubeVector;
	
	for( int color = RubiksCube::YELLOW; color <= RubiksCube::ORANGE; color++ )
	{
		// Go collect all face cubes of a certain color.
		subCubeVector.clear();
		situationStack->Top()->rubiksCube->CollectSubCubes( ( RubiksCube::Color* )&color, 1, subCubeVector );
		wxASSERT( subCubeVector.size() == 4 );

		// Now go see which pairs of these face cubes are potentially pairable.
		// If I'm not mistaken, if consider one face cube against another, then this
		// is equivilant to considering the second against the first, no matter the
		// perspective, so here, these are non-ordered pairs.
		for( int i = 0; i < 4; i++ )
		{
			const RubiksCube::SubCube* subCube0 = subCubeVector[i];
			if( !IsSolitaryFaceCube( subCube0 ) )
				continue;

			for( int j = i + 1; j < 4; j++ )
			{
				const RubiksCube::SubCube* subCube1 = subCubeVector[j];
				if( !IsSolitaryFaceCube( subCube1 ) )
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
bool SolverForCase4::IsSolitaryFaceCube( const RubiksCube::SubCube* subCube )
{
	FacePairList facePairListForSubCube;
	CollectFacePairsForSubCube( subCube, facePairListForSubCube );
	int count = facePairListForSubCube.size();
	wxASSERT( 0 <= count && count <= 2 );
	if( count == 0 )
		return true;
	if( count == 2 )
		return false;

	// If the count is 1, then the sub-cube is solitary if the face-pair of which
	// it is a member overlaps some other face-pair.
	const Situation* situation = ( const Situation* )situationStack->Top();
	const FacePairList& facePairList = situation->facePairList;
	for( FacePairList::const_iterator iter = facePairList.begin(); iter != facePairList.end(); iter++ )
	{
		FacePair facePair = *iter;
		if( FacePairsOverlapAndAreDistinct( facePair, facePairListForSubCube.front() ) )
			return true;
	}

	return false;
}

//==================================================================================================
bool SolverForCase4::AreEdgePairsSolved( void )
{
	return false;
}

//==================================================================================================
bool SolverForCase4::SolveEdgePairs( RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
bool SolverForCase4::SolveAs3x3x3( RubiksCube::RotationSequence& rotationSequence )
{
	return false;
}

//==================================================================================================
void SolverForCase4::MakeFacePairList( FacePairList& facePairList )
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
			if( IsFacePairValid( facePair ) )
				facePairList.push_back( facePair );
		}
	}
}

//==================================================================================================
bool SolverForCase4::IsFacePairValid( const FacePair& facePair )
{
	RubiksCube::Color colors[2];
	if( !FacePairColors( facePair, colors ) )
		return false;
	if( colors[0] != colors[1] )
		return false;
	return true;
}

//==================================================================================================
bool SolverForCase4::FacePairColors( const FacePair& facePair, RubiksCube::Color* colors )
{
	const RubiksCube::SubCube* subCubes[2];
	if( !FacePairSubCubes( facePair, subCubes ) )
		return false;

	colors[0] = subCubes[0]->faceData[ facePair.face ].color;
	colors[1] = subCubes[1]->faceData[ facePair.face ].color;
	return true;
}

//==================================================================================================
bool SolverForCase4::FacePairSubCubes( const FacePair& facePair, const RubiksCube::SubCube** subCubes )
{
	subCubes[0] = 0;
	subCubes[1] = 0;

	const RubiksCube* rubiksCube = situationStack->Top()->rubiksCube;

	switch( facePair.face )
	{
		case RubiksCube::NEG_X:
		case RubiksCube::POS_X:
		{
			int x = ( facePair.face == RubiksCube::NEG_X ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::Y_AXIS )
			{
				subCubes[0] = rubiksCube->Matrix( RubiksCube::Coordinates( x, facePair.plane.index, 1 ) );
				subCubes[1] = rubiksCube->Matrix( RubiksCube::Coordinates( x, facePair.plane.index, 2 ) );
			}
			else if( facePair.plane.axis == RubiksCube::Z_AXIS )
			{
				subCubes[0] = rubiksCube->Matrix( RubiksCube::Coordinates( x, 1, facePair.plane.index ) );
				subCubes[1] = rubiksCube->Matrix( RubiksCube::Coordinates( x, 2, facePair.plane.index ) );
			}
			break;
		}
		case RubiksCube::NEG_Y:
		case RubiksCube::POS_Y:
		{
			int y = ( facePair.face == RubiksCube::NEG_Y ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::X_AXIS )
			{
				subCubes[0] = rubiksCube->Matrix( RubiksCube::Coordinates( facePair.plane.index, y, 1 ) );
				subCubes[1] = rubiksCube->Matrix( RubiksCube::Coordinates( facePair.plane.index, y, 2 ) );
			}
			else if( facePair.plane.axis == RubiksCube::Z_AXIS )
			{
				subCubes[0] = rubiksCube->Matrix( RubiksCube::Coordinates( 1, y, facePair.plane.index ) );
				subCubes[1] = rubiksCube->Matrix( RubiksCube::Coordinates( 2, y, facePair.plane.index ) );
			}
			break;
		}
		case RubiksCube::NEG_Z:
		case RubiksCube::POS_Z:
		{
			int z = ( facePair.face == RubiksCube::NEG_Z ) ? 0 : 3;
			if( facePair.plane.axis == RubiksCube::X_AXIS )
			{
				subCubes[0] = rubiksCube->Matrix( RubiksCube::Coordinates( facePair.plane.index, 1, z ) );
				subCubes[1] = rubiksCube->Matrix( RubiksCube::Coordinates( facePair.plane.index, 2, z ) );
			}
			else if( facePair.plane.axis == RubiksCube::Y_AXIS )
			{
				subCubes[0] = rubiksCube->Matrix( RubiksCube::Coordinates( 1, facePair.plane.index, z ) );
				subCubes[1] = rubiksCube->Matrix( RubiksCube::Coordinates( 2, facePair.plane.index, z ) );
			}
			break;
		}
	}

	if( subCubes[0] && subCubes[1] )
		return true;

	return false;
}

//==================================================================================================
/*static*/ bool SolverForCase4::FacePairsOverlapAndAreDistinct( const FacePair& facePair0, const FacePair& facePair1 )
{
	wxASSERT( &facePair0 != &facePair1 );

	// A pair of face-pairs can't overlap unless they share the same face.
	if( facePair0.face != facePair1.face )
		return false;

	if( facePair0.plane.axis == facePair1.plane.axis )
	{
		// If the face-pairs are the same, then they're not distinct.
		// If the face-pairs are distinct, but have the same axis, then they are parallel and can't overlap.
		return false;
	}

	return true;
}

//==================================================================================================
void SolverForCase4::RotationThatPreservesFacePairs( RubiksCube::Face face, const RubiksCube::Rotation& anticipatedRotation, RubiksCube::Rotation& rotation )
{
	rotation.angle = 0.0;
	rotation.plane = situationStack->Top()->rubiksCube->TranslateFace( face );

	FacePairList facePairsForFace;
	CollectFacePairsForFace( face, facePairsForFace );

	if( facePairsForFace.size() == 4 || facePairsForFace.size() == 0 )
		return;

	if( facePairsForFace.size() == 2 )
	{
		FacePairList::iterator iter = facePairsForFace.begin();
		FacePair facePair0 = *iter++;
		FacePair facePair1 = *iter;
		if( FacePairsOverlapAndAreDistinct( facePair0, facePair1 ) || !AnticipatedRotationSplitsPair( anticipatedRotation, facePair0 ) )
			return;
	}

	if( facePairsForFace.size() == 1 )
	{
		FacePairList::iterator iter = facePairsForFace.begin();
		FacePair facePair = *iter;
		if( !AnticipatedRotationSplitsPair( anticipatedRotation, facePair ) )
			return;
	}

	// If we reach here, then a rotation is necessary.  Any quarter turn will do.  Randomly choose?
	rotation.angle = M_PI / 2.0;
}

//==================================================================================================
// Notice that we don't check if the given rotation is a zero-angle rotation.
/*static*/ bool SolverForCase4::AnticipatedRotationSplitsPair( const RubiksCube::Rotation& anticipatedRotation, const FacePair& facePair )
{
	if( anticipatedRotation.plane.index == 0 || anticipatedRotation.plane.index == 3 )
		return false;
	if( facePair.plane.axis == anticipatedRotation.plane.axis )
		return false;
	return true;
}

//==================================================================================================
void SolverForCase4::CollectFacePairsForFace( RubiksCube::Face face, FacePairList& facePairListForFace )
{
	const Situation* situation = ( const Situation* )situationStack->Top();
	const FacePairList& facePairList = situation->facePairList;

	facePairListForFace.clear();
	for( FacePairList::const_iterator iter = facePairList.begin(); iter != facePairList.end(); iter++ )
	{
		FacePair facePair = *iter;
		if( facePair.face == face )
			facePairListForFace.push_back( facePair );
	}
}

//==================================================================================================
bool SolverForCase4::CollectFacePairForFaceAndPlane( RubiksCube::Face face, const RubiksCube::Plane& plane, FacePair& facePair )
{
	FacePairList facePairListForFace;
	CollectFacePairsForFace( face, facePairListForFace );

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
void SolverForCase4::CollectFacePairsForSubCube( const RubiksCube::SubCube* subCube, FacePairList& facePairListForSubCube )
{
	const Situation* situation = ( const Situation* )situationStack->Top();
	const FacePairList& facePairList = situation->facePairList;

	facePairListForSubCube.clear();
	for( FacePairList::const_iterator iter = facePairList.begin(); iter != facePairList.end(); iter++ )
	{
		FacePair facePair = *iter;
		const RubiksCube::SubCube* subCubes[2];
		FacePairSubCubes( facePair, subCubes );
		if( subCubes[0] == subCube || subCubes[1] == subCube )
			facePairListForSubCube.push_back( facePair );
	}
}

// SolverForCase4.cpp