// SolverForCase3.h

//==================================================================================================
// Here we implement the layer-solve strategy.
class SolverForCase3 : public Solver
{
public:

	enum ParityError
	{
		ERROR_PARITY_FIX_WITH_EDGE_SWAP,
		ERROR_PARITY_FIX_WITH_EDGE_FLIP,
	};

	typedef std::list< ParityError > ParityErrorList;

	SolverForCase3( ParityErrorList* parityErrorList = 0 );
	virtual ~SolverForCase3( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

private:

	void PerformCubeOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCrossPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCornersPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCornersOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformMiddleEdgePositioningAndOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCrossAndCornersRelativePositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCornerOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCrossAndCornersPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );

	static RubiksCube::Perspective standardPerspectives[4];
	static RubiksCube::Perspective standardPerspectivesNegated[4];

	static RubiksCube::Color redEdgeColors[4][2];
	static RubiksCube::Color redCornerColors[4][3];
	static RubiksCube::Color middleEdgeColors[4][2];
	static RubiksCube::Color orangeEdgeColors[4][2];
	static RubiksCube::Color orangeCornerColors[4][3];

	static int redEdgeTargetLocations[4][3];
	static int redCornerTargetLocations[4][3];
	static int middleEdgeTargetLocations[4][3];
	static int orangeEdgeTargetLocations[4][3];
	static int orangeCornerTargetLocations[4][3];

	static c3ga::vectorE3GA xAxis, yAxis, zAxis;

	static double CalculateRotationAngle( const c3ga::vectorE3GA& unitVec0, const c3ga::vectorE3GA& unitVec1, const c3ga::vectorE3GA& axis, double epsilon = 1e-7 );

	static void AppendZeroRotation( RubiksCube::RotationSequence& rotationSequence );

	ParityErrorList* parityErrorList;
};

// SolverForCase3.h