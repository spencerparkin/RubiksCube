// Application.h

//==================================================================================================
class Application : public wxApp
{
public:

	Application( void );
	virtual ~Application( void );

	virtual bool OnInit( void ) override;
	virtual int OnExit( void ) override;

	RubiksCube* rubiksCube;

	typedef std::list< RubiksCube::Rotation > MoveStack;
	MoveStack moveStack;

	Solver* solver;
};

//==================================================================================================
wxDECLARE_APP( Application );

// Application.h