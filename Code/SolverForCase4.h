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
	};

	typedef std::list< FacePair > FacePairList;

	// The perspective here will always have subCube[0] on the right-axis face,
	// and subCube[1] on the up-axis face.
	struct PotentialFacePair
	{
		const RubiksCube::SubCube* subCube[2];
		RubiksCube::Face face[2];
		RubiksCube::Perspective perspective;
	};

	typedef std::list< PotentialFacePair > PotentialFacePairList;

	// struct EdgePair { ... };

	static bool AreFacesSolved( const RubiksCube& rubiksCube, const FacePairList& facePairList );
	static bool AreFacePairsSolved( const RubiksCube& rubiksCube, const FacePairList& facePairList );
	static bool AreEdgePairsSolved( const RubiksCube& rubiksCube );

	static bool SolveFaces( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	static bool SolveFacePairs( const RubiksCube& rubiksCube, const FacePairList& facePairList, RubiksCube::RotationSequence& rotationSequence );
	static void SolveFacePairing( const RubiksCube& rubiksCube, const FacePairList& facePairList, const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence );
	static bool SolveEdgePairs( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	//static void SolveEdgePairing(...);
	static bool SolveAs3x3x3( const RubiksCube& rubiksCube, RubiksCube::RotationSequence& rotationSequence );

	static void PrepareFacePairing( const RubiksCube& rubiksCube, const FacePairList& facePairList, const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence );
	//static void PrepareEdgePairing( const RubiksCube& rubiksCube, const FacePairList& facePairList, const PotentialFacePairing& potentialFacePairing, RubiksCube::RotationSequence& rotationSequence );

	static void MakeFacePairList( const RubiksCube& rubiksCube, FacePairList& facePairList );
	static bool FacePairsOverlap( const FacePair& facePair0, const FacePair& facePair1 );
	static void RotationThatPreservesFacePairs( const RubiksCube& rubiksCube, RubiksCube::Face face, const FacePairList& facePairList, const RubiksCube::Rotation& anticipatedRotation, RubiksCube::Rotation& rotation );
	static void CollectFacePairsForFace( RubiksCube::Face face, const FacePairList& facePairList, FacePairList& facePairListForFace );
	static bool CollectFacePairForFaceAndPlane( RubiksCube::Face face, const RubiksCube::Plane& plane, const FacePairList& facePairList, FacePair& facePair );
	static void CollectFacePairsForSubCube( const RubiksCube& rubiksCube, const RubiksCube::SubCube* subCube, const FacePairList& facePairList, FacePairList& facePairListForSubCube );
	static bool FacePairColors( const RubiksCube& rubiksCube, const FacePair& facePair, RubiksCube::Color* colors );
	static bool FacePairSubCubes( const RubiksCube& rubiksCube, const FacePair& facePair, const RubiksCube::SubCube** subCubes );
	static bool IsFacePairValid( const RubiksCube& rubiksCube, const FacePair& facePair );
	static bool AnticipatedRotationSplitsPair( const RubiksCube::Rotation& anticipatedRotation, const FacePair& facePair );

	static void FindAllPotentialFacePairings( const RubiksCube& rubiksCube, const FacePairList& facePairList, PotentialFacePairList& potentialFacePairList );
	//static void FindAllPotentialEdgeParings(...);

	static bool IsSolitaryFaceCube( const RubiksCube& rubiksCube, const RubiksCube::SubCube* subCube, const FacePairList& facePairList );
	//static bool IsSolitaryEdgeCube( const RubiksCube::SubCube* subCube, ... );

	static bool FindFacePairPreservationSequence( SituationStack& situationStack, const FacePair& potentialSplitPair, const RubiksCube::Rotation& anticipatedRotation, const RubiksCube::Perspective& perspective, RubiksCube::RotationSequence& preservationSequence );

	static int PlacementForFacePairingSolver( const RubiksCube::Coordinates& relativeCoords );
};

// SolverForCase4.h