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
		ID_RenderWithPerspectiveProjection,
		ID_RenderWithOrthographicProjection,
		ID_ShowPerspectiveLabels,
		ID_PopRotationAndApply,
		ID_PopRotationAndNoApply,
		ID_Exit,
		ID_About,
		ID_Timer,
	};

	void OnCreateCube( wxCommandEvent& event );
	void OnDestroyCube( wxCommandEvent& event );
	void OnScrambleCube( wxCommandEvent& event );
	void OnSolveCube( wxCommandEvent& event );
	void OnRenderWithPerspectiveProjection( wxCommandEvent& event );
	void OnRenderWithOrthographicProjection( wxCommandEvent& event );
	void OnShowPerspectiveLabels( wxCommandEvent& event );
	void OnPopRotationAndApply( wxCommandEvent& event );
	void OnPopRotationAndNoApply( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
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
};

// Frame.h