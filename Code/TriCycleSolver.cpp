// TriCycleSolver.cpp

#include "Header.h"

//==================================================================================================
// Find the smallest tri-cycle sequence that orders the first quad in the same order as the second.
// We asume here that the second quad is in order as we only search for tri-cycle sequences that
// order the first quad in some way, and then compare those against the order of the second quad.
/*static*/ bool TriCycleSolver::FindSmallestTriCycleSequenceThatOrdersQuadTheSame( const int* firstQuad, const int* secondQuad, TriCycleSequence& triCycleSequence )
{
	bool found = false;
	wxASSERT( IsQuadInOrder( secondQuad ) );

	TriCycleSequenceList triCycleSequenceList;
	FindAllTriCycleSequencesThatOrderQuad( firstQuad, triCycleSequenceList, 3 );

	TriCycleSequence* smallestTriCycleSequence = 0;
	for( TriCycleSequenceList::iterator iter = triCycleSequenceList.begin(); iter != triCycleSequenceList.end(); iter++ )
	{
		TriCycleSequence* triCycleSequence = *iter;

		int localQuad[4];
		ApplyTriCycleSequenceToQuad( firstQuad, localQuad, *triCycleSequence );
		wxASSERT( IsQuadInOrder( localQuad ) );
		if( AreQuadsInSameOrder( localQuad, secondQuad ) )
		{
			if( !smallestTriCycleSequence || smallestTriCycleSequence->size() > triCycleSequence->size() )
				smallestTriCycleSequence = triCycleSequence;
		}
	}

	if( smallestTriCycleSequence )
	{
		triCycleSequence.assign( smallestTriCycleSequence->begin(), smallestTriCycleSequence->end() );
		found = true;
	}

	DeleteTriCycleSequenceList( triCycleSequenceList );

	return found;
}

//==================================================================================================
/*static*/ bool TriCycleSolver::FindSmallestTriCycleSequenceThatOrdersQuad( const int* quad, TriCycleSequence& triCycleSequence )
{
	bool found = false;

	TriCycleSequenceList triCycleSequenceList;
	FindAllTriCycleSequencesThatOrderQuad( quad, triCycleSequenceList, 3 );

	TriCycleSequence* smallestTriCycleSequence = 0;
	for( TriCycleSequenceList::iterator iter = triCycleSequenceList.begin(); iter != triCycleSequenceList.end(); iter++ )
	{
		TriCycleSequence* triCycleSequence = *iter;
		if( !smallestTriCycleSequence || smallestTriCycleSequence->size() > triCycleSequence->size() )
			smallestTriCycleSequence = triCycleSequence;
	}

	if( smallestTriCycleSequence )
	{
		triCycleSequence.assign( smallestTriCycleSequence->begin(), smallestTriCycleSequence->end() );
		found = true;
	}

	DeleteTriCycleSequenceList( triCycleSequenceList );

	return found;
}

//==================================================================================================
/*static*/ void TriCycleSolver::DeleteTriCycleSequenceList( TriCycleSequenceList& triCycleSequenceList )
{
	while( triCycleSequenceList.size() > 0 )
	{
		TriCycleSequenceList::iterator iter = triCycleSequenceList.begin();
		TriCycleSequence* triCycleSequence = *iter;
		delete triCycleSequence;
		triCycleSequenceList.erase( iter );
	}
}

//==================================================================================================
/*static*/ void TriCycleSolver::FindAllTriCycleSequencesThatOrderQuad( const int* quad, TriCycleSequenceList& triCycleSequenceList, int maxSequenceLength )
{
	TriCycleSequence triCycleSequence;
	FindAllTriCycleSequencesThatOrderQuad( quad, triCycleSequenceList, triCycleSequence, maxSequenceLength );
}

//==================================================================================================
/*static*/ void TriCycleSolver::FindAllTriCycleSequencesThatOrderQuad( const int* quad, TriCycleSequenceList& triCycleSequenceList, TriCycleSequence& triCycleSequence, int depthCountDown )
{
	if( IsQuadInOrder( quad ) )
	{
		TriCycleSequence* newTriCycleSequence = new TriCycleSequence();
		newTriCycleSequence->assign( triCycleSequence.begin(), triCycleSequence.end() );
		triCycleSequenceList.push_back( newTriCycleSequence );
	}

	if( depthCountDown > 0 )
	{
		for( int index = 0; index < 8; index++ )
		{
			TriCycle triCycle;
			triCycle.invariantIndex = index / 2;
			triCycle.direction = TriCycle::Direction( index % 2 );

			triCycleSequence.push_back( triCycle );

			int localQuad[4];
			ApplyTriCycleToQuad( quad, localQuad, triCycle );
			FindAllTriCycleSequencesThatOrderQuad( localQuad, triCycleSequenceList, triCycleSequence, depthCountDown - 1 );

			triCycleSequence.pop_back();
		}
	}
}

//==================================================================================================
/*static*/ void TriCycleSolver::ApplyTriCycleSequenceToQuad( const int* quad, int* triCycledQuad, const TriCycleSequence& triCycleSequence )
{
	int localQuad[2][4];
	int indexIn = 0;
	int indexOut = 1;

	for( int index = 0; index < 4; index++ )
		localQuad[ indexIn ][ index ] = quad[ index ];

	for( TriCycleSequence::const_iterator iter = triCycleSequence.begin(); iter != triCycleSequence.end(); iter++ )
	{
		TriCycle triCycle = *iter;
		ApplyTriCycleToQuad( localQuad[ indexIn ], localQuad[ indexOut ], triCycle );
		indexIn = 1 - indexIn;
		indexOut = 1 - indexOut;
	}

	for( int index = 0; index < 4; index++ )
		triCycledQuad[ index ] = localQuad[ indexIn ][ index ];
}

//==================================================================================================
/*static*/ void TriCycleSolver::ApplyTriCycleToQuad( const int* quad, int* triCycledQuad, const TriCycle& triCycle )
{
	wxASSERT( quad != triCycledQuad );
	for( int index = 0; index < 4; index++ )
	{
		int cycledIndex = index;
		if( index != triCycle.invariantIndex )
		{
			int offset = ( triCycle.direction == TriCycle::BACKWARD ) ? 1 : 3;
			cycledIndex = ( index + offset ) % 4;
			if( cycledIndex == triCycle.invariantIndex )
				cycledIndex = ( cycledIndex + offset ) % 4;
		}

		triCycledQuad[ index ] = quad[ cycledIndex ];
	}
}

//==================================================================================================
/*static*/ bool TriCycleSolver::IsQuadInOrder( const int* quad )
{
	for( int index = 0; index < 4; index++ )
		if( ( quad[ index ] + 1 ) % 4 != quad[ ( index + 1 ) % 4 ] )
			return false;

	return true;
}

//==================================================================================================
/*static*/ bool TriCycleSolver::AreQuadsInSameOrder( const int* firstQuad, const int* secondQuad )
{
	for( int index = 0; index < 4; index++ )
		if( firstQuad[ index ] != secondQuad[ index ] )
			return false;

	return true;
}

// TriCycleSolver.cpp