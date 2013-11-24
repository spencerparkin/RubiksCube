// Solver.h

//==================================================================================================
class Solver
{
public:

	Solver( void );
	virtual ~Solver( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) = 0;
};

// Solver.h