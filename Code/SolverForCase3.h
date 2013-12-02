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
	void PerformOrangeCrossOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCrossPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCornerPositioningStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
	void PerformOrangeCornerOrientingStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );

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

	struct CycleAction
	{
		enum CycleType
		{
			NO_CYCLE,
			QUAD_CYCLE_FORWARD,
			QUAD_CYCLE_BACKWARD,
			TRI_CYCLE_FORWARD,
			TRI_CYCLE_BACKWARD,
		};

		CycleType cycleType;

		union
		{
			int triCycleInvariantIndex;
			int quadCycleCount;
		};
	};

	typedef std::list< CycleAction > CycleActionList;

	static void SolveQuadAndTriCycleProblem( int* order, CycleActionList& cycleActionList, bool solveCompletely = false );
};

// SolverForCase3.h