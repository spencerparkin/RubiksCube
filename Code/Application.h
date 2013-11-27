// Application.h

//==================================================================================================
class Application : public wxApp
{
public:

	Application( void );
	virtual ~Application( void );

	virtual bool OnInit( void ) override;
	virtual int OnExit( void ) override;

	void PushRotation( const RubiksCube::Rotation& rotation );
	bool PopRotation( RubiksCube::Rotation* rotation = 0 );

	enum { ROTATION_STACK_CAPACITY = 50 };

	RubiksCube* rubiksCube;

	RubiksCube::RotationSequence rotationStack;

	Solver* solver;
};

//==================================================================================================
wxDECLARE_APP( Application );

// Application.h