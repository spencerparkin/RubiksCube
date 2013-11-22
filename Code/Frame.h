// Frame.h

//==================================================================================================
class Frame : public wxFrame
{
public:

	Frame( wxWindow* parent );
	virtual ~Frame( void );

private:

	enum
	{
		ID_CreateCube = wxID_HIGHEST,
		ID_DestroyCube,
		ID_ScrambleCube,
		ID_PopMove,
		ID_Exit,
		ID_About,
		ID_Timer,
	};

	void OnCreateCube( wxCommandEvent& event );
	void OnDestroyCube( wxCommandEvent& event );
	void OnScrambleCube( wxCommandEvent& event );
	void OnPopMove( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void OnUpdateMenuItemUI( wxUpdateUIEvent& event );
	void OnTimer( wxTimerEvent& event );

	Canvas* canvas;
	wxTimer timer;
	Application::MoveStack scrambleList;
};

// Frame.h