// SolverForCaseGreaterThan3.h

//==================================================================================================
// Our strategy here is to reduce the NxNxN case, for N>3, to that of the 3x3x3 case.
class SolverForCaseGreaterThan3 : public Solver
{
public:

	SolverForCaseGreaterThan3( void );
	virtual ~SolverForCaseGreaterThan3( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

private:

	void TranslateRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::RotationSequence& reducedRotationSequence, RubiksCube::RotationSequence& rotationSequence );

	SolverForCase3* solverForCase3;
	RubiksCube* reducedCube;
};

// SolverForCaseGreaterThan3.h