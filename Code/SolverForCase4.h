// SolverForCase4.h

//==================================================================================================
class SolverForCase4 : public Solver
{
public:

	SolverForCase4( void );
	virtual ~SolverForCase4( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
};

// SolverForCase4.h