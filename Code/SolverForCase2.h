// SolverForCase2.h

//==================================================================================================
class SolverForCase2 : public Solver
{
public:

	SolverForCase2( RubiksCube* rubiksCube );
	virtual ~SolverForCase2( void );

	virtual bool MakeMoveSequence( MoveSequence& moveSequence ) override;
};

// SolverForCase2.h