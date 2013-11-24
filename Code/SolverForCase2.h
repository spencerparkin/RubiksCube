// SolverForCase2.h

//==================================================================================================
class SolverForCase2 : public Solver
{
public:

	SolverForCase2( void );
	virtual ~SolverForCase2( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
};

// SolverForCase2.h