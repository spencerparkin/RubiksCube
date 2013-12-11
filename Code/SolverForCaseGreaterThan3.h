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

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

private:

	void TranslateRotationSequence( const RubiksCube* rubiksCube, const RubiksCube::RotationSequence& reducedRotationSequence, RubiksCube::RotationSequence& rotationSequence );

	SolverForCase3* solverForCase3;
	RubiksCube* reducedCube;
};

// SolverForCaseGreaterThan3.h