// SolverForCaseGreaterThan3.h

//==================================================================================================
// Our strategy here is to reduce the NxNxN case, for N>3, to that of the 3x3x3 case.
// A monkey wrench thrown into this plan is that, even if the face groups and edge groups
// are completed in an NxNxN, the orientation of these may be such that it is not physically
// possible for a 3x3x3 to ever resemble it.  Still, a common method is to reduce the NxNxN
// case to a 3x3x3 cube, solve using the 3x3x3 methods, but then to do sometimes-necessary fix-ups
// that arrise because of the NxNxN freedoms not allowed in a 3x3x3.
class SolverForCaseGreaterThan3 : public Solver
{
public:

	SolverForCaseGreaterThan3( void );
	virtual ~SolverForCaseGreaterThan3( void );

	virtual bool GetReady( void ) override;

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

	static bool GenerateEdgeParityFixSequence( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, const std::vector< int >& planeIndexVector, RubiksCube::RotationSequence& rotationSequence );

	static void BatchRotate( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, const std::vector< int >& planeIndexVector, RubiksCube::RelativeRotation::Type type, int rotationCount, RubiksCube::RotationSequence& rotationSequence );

private:

	void TranslateRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::RotationSequence& reducedRotationSequence, RubiksCube::RotationSequence& rotationSequence );

	class StageSolver
	{
	public:

		enum State
		{
			STAGE_PENDING,
			STAGE_COMPLETE,
		};

		StageSolver( void );
		virtual ~StageSolver( void );

		virtual bool SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) = 0;
		virtual bool VerifyActuallySolved( const RubiksCube* rubiksCube ) { return true; }

		State GetState( void ) { return state; }
		void SetState( State state ) { this->state = state; }

	private:

		State state;
	};

	class CenterFaceSolver : public StageSolver
	{
	public:

		CenterFaceSolver( void );
		virtual ~CenterFaceSolver( void );

		virtual bool SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
	};

	class FaceSolver : public StageSolver
	{
	public:

		FaceSolver( RubiksCube::Face face );
		virtual ~FaceSolver( void );

		virtual bool SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
		virtual bool VerifyActuallySolved( const RubiksCube* rubiksCube ) override;

	private:

		typedef std::list< RubiksCube::Perspective > PerspectiveList;
		PerspectiveList perspectiveList;

		void GeneratePerspectiveList( void );
		bool FindRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, RubiksCube::RotationSequence& rotationSequence );
		bool GenerateRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::Perspective& perspective, RubiksCube::RotationSequence& rotationSequence, const RubiksCube::Coordinates& forwardCoords );

		RubiksCube::Face face;
	};

	class EdgeSolver : public StageSolver
	{
	public:

		EdgeSolver( RubiksCube::Color colorA, RubiksCube::Color colorB );
		virtual ~EdgeSolver( void );

		virtual bool SolveStage( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
		virtual bool VerifyActuallySolved( const RubiksCube* rubiksCube ) override;

	private:

		RubiksCube::Color colors[2];

		typedef std::list< RubiksCube::Perspective > PerspectiveList;
		PerspectiveList perspectiveList;
	};

	typedef std::list< StageSolver* > StageSolverList;
	StageSolverList stageSolverList;

	void CreateStageSolvers( void );
	void ResetStageSolvers( void );
	void DestroyStageSolvers( void );
};

// SolverForCaseGreaterThan3.h