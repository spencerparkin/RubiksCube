// SolverForCase4.h

//==================================================================================================
class SolverForCase4 : public Solver
{
public:

	SolverForCase4( void );
	virtual ~SolverForCase4( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

private:

	struct FacePair
	{
		RubiksCube::Plane plane;
		RubiksCube::Face face;
		RubiksCube::Color color;
	};

	typedef std::list< FacePair > FacePairList;

	static bool AreFacesSolved( const RubiksCube& rubiksCube, const FacePairList& facePairList );
	static bool AreFacePairsSolved( const RubiksCube& rubiksCube, const FacePairList& facePairList );
	static bool AreEdgesSolved( const RubiksCube& rubiksCube );

	static bool SolveFaces( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	static bool SolveFacePairs( const RubiksCube& rubiksCube, const FacePairList& facePairList, RubiksCube::RotationSequence& rotationSequence );
	static bool SolveEdges( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	static bool SolveAs3x3x3( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence );

	static void MakeFacePairList( const RubiksCube& rubiksCube, FacePairList& facePairList );
	static bool FacePairsOverlap( const FacePair& facePair0, const FacePair& facePair1 );
	static void RotationThatPreservesFacePairs( const RubiksCube& rubiksCube, RubiksCube::Face face, const FacePairList& facePairList, const RubiksCube::Rotation& anticipatedRotation, RubiksCube::Rotation& rotation );
	static void CollectFacePairsForFace( RubiksCube::Face face, const FacePairList& facePairList, FacePairList& facePairListForFace );
	static bool FacePairColors( const RubiksCube& rubiksCube, const FacePair& facePair, RubiksCube::Color* colors );
	static bool AnticipatedRotationSplitsPair( const RubiksCube::Rotation& anticipatedRotation, const FacePair& facePair );

	// struct EdgePair { ... };
};

// SolverForCase4.h