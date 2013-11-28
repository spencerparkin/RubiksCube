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

	static RubiksCube::Color redEdgeColors[4][2];
	static RubiksCube::Perspective redEdgePerspectives[4];
	static int redEdgeTargetLocations[4][3];
	static c3ga::vectorE3GA xAxis, yAxis, zAxis;
};

// SolverForCase3.h