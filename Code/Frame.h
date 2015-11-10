// Frame.h

//==================================================================================================
class Frame : public wxFrame
{
public:

	Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size );
	virtual ~Frame( void );

private:

	enum
	{
		ID_CreateCube = wxID_HIGHEST,
		ID_DestroyCube,
		ID_ScrambleCube,
		ID_SolveCube,
		ID_CubeInACube,
		ID_SaveCube,
		ID_LoadCube,
		ID_RenderWithPerspectiveProjection,
		ID_RenderWithOrthographicProjection,
		ID_ShowPerspectiveLabels,
		ID_ShowInvariantFaces,
		ID_TakeSnapShot,
		ID_RotationHistoryClear,
		ID_RotationHistoryGoForward,
		ID_RotationHistoryGoBackward,
		ID_Exit,
		ID_DebugMode,
		ID_SilentDebugMode,
		ID_About,
		ID_Timer,
	};

	void OnCreateCube( wxCommandEvent& event );
	void OnDestroyCube( wxCommandEvent& event );
	void OnScrambleCube( wxCommandEvent& event );
	void OnSolveCube( wxCommandEvent& event );
	void OnCubeInACube( wxCommandEvent& event );
	void OnSaveCube( wxCommandEvent& event );
	void OnLoadCube( wxCommandEvent& event );
	void OnRenderWithPerspectiveProjection( wxCommandEvent& event );
	void OnRenderWithOrthographicProjection( wxCommandEvent& event );
	void OnShowPerspectiveLabels( wxCommandEvent& event );
	void OnShowInvariantFaces( wxCommandEvent& event );
	void OnTakeSnapShot( wxCommandEvent& event );
	void OnRotationHistoryClear( wxCommandEvent& event );
	void OnRotationHistoryGoForward( wxCommandEvent& event );
	void OnRotationHistoryGoBackward( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnDebugMode( wxCommandEvent& event );
	void OnSilentDebugMode( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void OnUpdateMenuItemUI( wxUpdateUIEvent& event );
	void OnTimer( wxTimerEvent& event );
	void OnTextCtrlEnter( wxCommandEvent& event );

	static bool ParseRelativeRotationSequenceString( const wxString& relativeRotationSequenceString, RubiksCube::RelativeRotationSequence& relativeRotationSequence );

	Canvas* canvas;
	wxTextCtrl* textCtrl;
	wxTimer timer;

	RubiksCube::RotationSequence executionSequence;
	double animationTolerance;

	enum DebugMode
	{
		DEBUG_MODE_NONE,
		DEBUG_MODE_SCRAMBLE,
		DEBUG_MODE_SOLVE,
	};

	DebugMode debugMode;
};

// Frame.h