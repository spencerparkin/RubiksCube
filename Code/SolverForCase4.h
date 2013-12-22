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
	//typedef std::list< EdgePair > EdgePairList;

	SituationStack* situationStack;

	class Situation : public Solver::SituationStack::Situation
	{
	public:
		FacePairList facePairList;
		//EdgePairList edgePairList;
	};

	virtual SituationStack::Situation* CreateSituation( void ) override;
	virtual void PostPush( SituationStack::Situation* situation ) override;
	virtual void PrePop( SituationStack::Situation* situation ) override;

	bool AreFacesSolved( bool andInCorrectPlacements = true );
	bool AreFacePairsSolved( void );
	bool AreEdgePairsSolved( void );

	bool SolveFaces( RubiksCube::RotationSequence& rotationSequence );
	bool SolveFacePairs( RubiksCube::RotationSequence& rotationSequence );
	void SolveFacePairing( const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence );
	bool SolveEdgePairs( RubiksCube::RotationSequence& rotationSequence );
	//void SolveEdgePairing(...);
	bool SolveAs3x3x3( RubiksCube::RotationSequence& rotationSequence );

	void PrepareFacePairing( const PotentialFacePair& potentialFacePair, RubiksCube::RotationSequence& rotationSequence );
	//void PrepareEdgePairing( const PotentialFacePairing& potentialFacePairing, RubiksCube::RotationSequence& rotationSequence );

	void MakeFacePairList( FacePairList& facePairList );
	void RotationThatPreservesFacePairs( RubiksCube::Face face, const RubiksCube::Rotation& anticipatedRotation, RubiksCube::Rotation& rotation );
	void CollectFacePairsForFace( RubiksCube::Face face, FacePairList& facePairListForFace );
	bool CollectFacePairForFaceAndPlane( RubiksCube::Face face, const RubiksCube::Plane& plane, FacePair& facePair );
	void CollectFacePairsForSubCube( const RubiksCube::SubCube* subCube, FacePairList& facePairListForSubCube );
	bool FacePairColors( const FacePair& facePair, RubiksCube::Color* colors );
	bool FacePairSubCubes( const FacePair& facePair, const RubiksCube::SubCube** subCubes );
	bool IsFacePairValid( const FacePair& facePair );
	void CalculatePlacementsForPotentialFacePair( const PotentialFacePair& potentialFacePair, const RubiksCube::SubCube** subCubes, int* faceCubePlacements );

	void FindAllPotentialFacePairings( PotentialFacePairList& potentialFacePairList );
	//void FindAllPotentialEdgeParings(...);

	bool IsSolitaryFaceCube( const RubiksCube::SubCube* subCube );
	//bool IsSolitaryEdgeCube( const RubiksCube::SubCube* subCube );

	bool FindFacePairPreservationSequence( const FacePair& potentialSplitPair, const RubiksCube::Rotation& anticipatedRotation, const RubiksCube::Perspective& perspective, RubiksCube::RotationSequence& preservationSequence );

	// These methods do not use the current top of the situation stack, but all other do.
	static int PlacementForFacePairingSolver( const RubiksCube::Coordinates& relativeCoords );
	static bool FacePairsOverlapAndAreDistinct( const FacePair& facePair0, const FacePair& facePair1 );
	static bool AnticipatedRotationSplitsPair( const RubiksCube::Rotation& anticipatedRotation, const FacePair& facePair );
};

// SolverForCase4.h