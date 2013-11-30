// SolverForCase3.h

//==================================================================================================
// Here we implement the layer-solve strategy.
class SolverForCase3 : public Solver
{
public:

	SolverForCase3( void );
	virtual ~SolverForCase3( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

private:

	void PerformCubeOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCrossPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCornersPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformRedCornersOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformMiddleEdgePositioningAndOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );

	static RubiksCube::Perspective redEdgeOrCornerPerspectives[4];
	static RubiksCube::Perspective middleEdgePerspectives[4];

	static RubiksCube::Color redEdgeColors[4][2];
	static RubiksCube::Color redCornerColors[4][3];
	static RubiksCube::Color middleEdgeColors[4][2];

	static int redEdgeTargetLocations[4][3];
	static int redCornerTargetLocations[4][3];
	static int middleEdgeTargetLocations[4][3];

	static c3ga::vectorE3GA xAxis, yAxis, zAxis;
};

// SolverForCase3.h