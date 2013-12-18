// Solver.h

//==================================================================================================
class Solver
{
public:

	Solver( void );
	virtual ~Solver( void );

	// A return value here of false indicates an error or failure on the part of the solver to solve the given cube.
	// If true and a sequence of size zero is returned, then the solver is finished solving the cube.
	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) = 0;

	bool MakeEntireSolutionSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence );

protected:

	// A situation stack is a tool that a solver class derivative may or may not choose to utilize.
	// Some solvers can return a rotation sequence to flush those moves out so that it can continue
	// on in its logic.  If, however, the algorithm for determining a move sequence is more sophisticated,
	// it may need to flush moves out on its own before it can continue on its logic and then at some point
	// return a rotation sequence.  This is often the case when trial-and-error (searching) is involved.
	class SituationStack
	{
	public:

		struct Situation
		{
			RubiksCube* rubiksCube;
			RubiksCube::RotationSequence rotationSequence;
		};

		typedef std::list< Situation > SituationList;

		SituationStack( const RubiksCube* rubiksCube );
		~SituationStack( void );

		const Situation* Top( void );
		void Push( const RubiksCube::Rotation& rotation );
		void Push( const RubiksCube::RotationSequence& rotationSequence );
		void Pop( void );

	private:
		SituationList situationList;
		const RubiksCube* rubiksCube;
	};
};

// Solver.h