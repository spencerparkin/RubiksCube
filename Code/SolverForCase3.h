// SolverForCase3.h

//==================================================================================================
class SolverForCase3 : public Solver
{
public:

	SolverForCase3( void );
	virtual ~SolverForCase3( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
};

// SolverForCase3.h