// SolverForCase3.h

//==================================================================================================
// Here we implement the layer-solve strategy.
class SolverForCase3 : public Solver
{
public:

	//==================================================================================================
	// If the solver is given an invalid 3x3x3, (a 3x3x3 cube that was assembled incorrectly),
	// here we can call upon a resolver to fix the problem.  This can happen if the calling code
	// is trying to solve a 4x4x4 as a 3x3x3 in the case that the 4x4x4 case does or almosts reduces
	// to the case of the 3x3x3.  The caller into our code may be solving a 4x4x4 as we go along
	// solving our 3x3x3, and so when we call the resolver, it can fix our 3x3x3, but then do sequences
	// on the 4x4x4 that do the equivilant fix.  Such fixes that I know of, are highly surgical in that
	// they fix only the parity problem while leaving everything else invariant.
	class InvalidCubeResolver
	{
	public:
		virtual void SwapEdgeOrientation( RubiksCube* rubiksCube, const RubiksCube::SubCube* edgeSubCube ) = 0;
		virtual void SwapEdges( RubiksCube* rubiksCube, const RubiksCube::SubCube* edgeSubCube0, const RubiksCube::SubCube* edgeSubCube1 ) = 0;
	};

	SolverForCase3( InvalidCubeResolver* invalidCubeResolver = 0 );
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

	InvalidCubeResolver* invalidCubeResolver;
};

// SolverForCase3.h