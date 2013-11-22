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
};

//==================================================================================================
wxDECLARE_APP( Application );

// Application.h