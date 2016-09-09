// Application.h

//==================================================================================================
class Application : public wxApp
{
public:

	Application( void );
	virtual ~Application( void );

	virtual bool OnInit( void ) override;
	virtual int OnExit( void ) override;

	void RotationHistoryAppend( const RubiksCube::Rotation& rotation );
	bool RotationHistoryGoForward( RubiksCube::Rotation& rotation );
	bool RotationHistoryGoBackward( RubiksCube::Rotation& rotation );
	bool RotationHistoryCanGoForward( void );
	bool RotationHistoryCanGoBackward( void );
	void RotationHistoryClear( void );

	wxImage* LoadTextureResource( const wxString& textureFile );

	RubiksCube* rubiksCube;
	bool makeInitialCube;

private:

	typedef std::vector< RubiksCube::Rotation > RotationHistory;
	RotationHistory rotationHistory;
	int rotationHistoryIndex;
};

//==================================================================================================
wxDECLARE_APP( Application );

// Application.h