// SolverForCase3.h

//==================================================================================================
class SolverForCase3 : public Solver
{
public:

	SolverForCase3( RubiksCube* rubiksCube );
	virtual ~SolverForCase3( void );

	virtual bool MakeMoveSequence( MoveSequence& moveSequence ) override;
};

// SolverForCase3.h