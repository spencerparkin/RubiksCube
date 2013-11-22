// Solver.h

//==================================================================================================
class Solver
{
public:

	Solver( RubiksCube* rubiksCube );
	virtual ~Solver( void );

	bool MakeMove( RubiksCube::Rotation& rotation );

	typedef std::list< RubiksCube::Rotation > MoveSequence;

	virtual bool MakeMoveSequence( MoveSequence& moveSequence ) = 0;

protected:

	RubiksCube* rubiksCube;
	MoveSequence currentMoveSequence;
};

// Solver.h