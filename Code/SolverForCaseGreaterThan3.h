// SolverForCaseGreaterThan3.h

//==================================================================================================
// Our strategy here is to reduce the NxNxN case, for N>3, to that of the 3x3x3 case.
class SolverForCaseGreaterThan3 : public Solver
{
public:

	SolverForCaseGreaterThan3( RubiksCube* rubiksCube );
	virtual ~SolverForCaseGreaterThan3( void );

	virtual bool MakeMoveSequence( MoveSequence& moveSequence ) override;

private:

	void TranslateMoveSequence( const MoveSequence& reducedMoveSequence, MoveSequence& moveSequence );

	SolverForCase3* solverForCase3;
	RubiksCube* reducedCube;
};

// SolverForCaseGreaterThan3.h