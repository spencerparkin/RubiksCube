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
	// Solvers work by returning sequences until no new sequence needs to be returned.  Between requests
	// for sequences, the solver may lose the context of what it was trying to figure out, so during
	// the handling of such a request, it may choose to use a situation stack to flush intermediate moves
	// to continue on in its logic.  The best example of this is that of searching for a good sequence
	// to return to the requester.
	class SituationStack
	{
	public:

		class Situation
		{
		public:
			Situation( void );
			virtual ~Situation( void );

			RubiksCube* rubiksCube;
			RubiksCube::RotationSequence rotationSequence;

			virtual void PostPush( Solver* solver );
			virtual void PrePop( Solver* solver );
		};

		typedef std::list< Situation* > SituationList;

		SituationStack( Solver* solver, const RubiksCube* rubiksCube );
		~SituationStack( void );

		const Situation* Top( void );
		void Push( const RubiksCube::Rotation& rotation );
		void Push( const RubiksCube::RotationSequence* rotationSequence = 0 );
		void Pop( void );
		void AppendRotationSequence( RubiksCube::RotationSequence& rotationSequence ) const;
		int Size( void ) const;

	private:

		Solver* solver;
		const RubiksCube* rubiksCube;
		
		SituationList situationList;
	};

	virtual SituationStack::Situation* CreateSituation( void );
	virtual void PostPush( SituationStack::Situation* situation );
	virtual void PrePop( SituationStack::Situation* situation );
};

// Solver.h