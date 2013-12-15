// SolverForCase4.h

//==================================================================================================
class SolverForCase4 : public Solver
{
public:

	SolverForCase4( void );
	virtual ~SolverForCase4( void );

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

private:

	//==================================================================================================
	class Phase
	{
	public:
		virtual bool IsComplete( void ) = 0;
		virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) = 0;
	};

	//==================================================================================================
	// Phase 1: Get the yellow (-Y) and white (+Y) faces complete.
	//   Step 1: Get 2 whites paired and 2 yellows paired.
	//   Step 2: Get 1 white pair in (+Y) face and 1 white pair in (-Y) face.
	//   Step 3: Get 1 yellow pair in (+Y) face and 1 yellow pair in (-Y) face.
	//   Step 4: Easy moves to get white face and yellow face complete.
	class PhaseOne : public Phase
	{
	public:
		virtual bool IsComplete( void ) override;
		virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;
	};

	//==================================================================================================
	// Phase 2: Get the (+/-X) and (+/-Z) faces one solid color.
	//   Step 1: Pair remaining colors in faces.
	//   Step 2: Make largest chain and combine to complete as many faces as possible.
	//   Step 3: Examine case to find quickest way to complete all faces.

	//==================================================================================================
	// Phase 3: Order middle faces correctly.
	//   Step 1: Is there an easy move sequence that swaps two adjacent faces?  If so, that's the easiest solution.
	//           If not, tri-cycling faces while keeping one invariant is possible, as well as swapping opposite faces.

	//==================================================================================================
	// Phase 4: Complete and park 4 edges in the (+Y) face, and 4 edge in the (-Y) face.

	//==================================================================================================
	// Phase 5: Complete the middle edges.

	//==================================================================================================
	// Phase 6: Solve as 3x3x3.
	//   Can we make the 3x3x3 solver able to handle an improperly built 3x3x3?
	//   Edges are not the only problem.  The final layer of the 3x3x3 (in the layer solve method),
	//   may not be solvable unless we perform a 4x4x4 sequence that makes it solvable.  Perhaps the
	//   3x3x3 solver should be up-graded to take a call-back that can be called to fix oddities in
	//   the cube as it encounters them.  The 4x4x4 move sequences that fix these oddities can do
	//   so while leaving the rest of the cube invariant.

	Phase** phase;
	int phaseCount;
};

// SolverForCase4.h