// TriCycleSolver.h

//==================================================================================================
// I'm sad to say that this is just a brute-force attack on the problem.  I'm not sure how to
// go about analytically solving this problem.  Group theory?  Anyhow, the combinatorics of the
// problem isn't so big that the computer can't manage it quickly.  Other parts of the Rubik's Cube
// solving code just break things down into cases, which is feasible when there aren't too many of them,
// but here, if we were to break things down into cases, there would just be too many cases to keep straight.
class TriCycleSolver
{
public:

	struct TriCycle
	{
		enum Direction
		{
			FORWARD,
			BACKWARD,
		};

		Direction direction;
		int invariantIndex;
	};

	typedef std::list< TriCycle > TriCycleSequence;
	typedef std::list< TriCycleSequence* > TriCycleSequenceList;

	static bool FindSmallestTriCycleSequenceThatOrdersQuadTheSame( const int* firstQuad, const int* secondQuad, TriCycleSequence& triCycleSequence );
	static bool FindSmallestTriCycleSequenceThatOrdersQuad( const int* quad, TriCycleSequence& triCycleSequence );
	static void FindAllTriCycleSequencesThatOrderQuad( const int* quad, TriCycleSequenceList& triCycleSequenceList, int maxSequenceLength );
	static void ApplyTriCycleSequenceToQuad( const int* quad, int* triCycledQuad, const TriCycleSequence& triCycleSequence );
	static void ApplyTriCycleToQuad( const int* quad, int* triCycledQuad, const TriCycle& triCycle );
	static bool IsQuadInOrder( const int* quad );
	static bool AreQuadsInSameOrder( const int* firstQuad, const int* secondQuad );
	static void DeleteTriCycleSequenceList( TriCycleSequenceList& triCycleSequenceList );

private:

	static void FindAllTriCycleSequencesThatOrderQuad( const int* quad, TriCycleSequenceList& triCycleSequenceList, TriCycleSequence& triCycleSequence, int depthCountDown );
};

// TriCycleSolver.h