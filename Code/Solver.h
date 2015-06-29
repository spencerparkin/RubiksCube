// Solver.h

//==================================================================================================
class Solver
{
public:

	Solver( void );
	virtual ~Solver( void );

	virtual bool GetReady( void ) { return true; }

	// A return value here of false indicates an error or failure on the part of the solver to solve the given cube.
	// If true and a sequence of size zero is returned, then the solver is finished solving the cube.
	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) = 0;

	bool MakeEntireSolutionSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );
};

// Solver.h