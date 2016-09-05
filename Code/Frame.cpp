// Frame.cpp

#include "Header.h"

//==================================================================================================
Frame::Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size ) : wxFrame( parent, wxID_ANY, "Rubik's Cube", pos, size ), timer( this, ID_Timer )
{
	debugMode = DEBUG_MODE_NONE;
	
	animationTolerance = 0.01;

	wxMenu* programMenu = new wxMenu();
	wxMenuItem* createCubeMenuItem = new wxMenuItem( programMenu, ID_CreateCube, "Create Cube", "Create a new Rubik's Cube." );
	wxMenuItem* destroyCubeMenuItem = new wxMenuItem( programMenu, ID_DestroyCube, "Destroy Cube", "Destroy the Rubik's Cube." );
	wxMenuItem* scrambleCubeMenuItem = new wxMenuItem( programMenu, ID_ScrambleCube, "Scramble Cube", "Randomize the Rubik's Cube." );
	wxMenuItem* solveCubeMenuItem = new wxMenuItem( programMenu, ID_SolveCube, "Solve Cube", "Solve the Rubik's Cube." );
	wxMenuItem* cubeInACubeMenuItem = new wxMenuItem( programMenu, ID_CubeInACube, "Cube In A Cube", "Solve for the cube-in-a-cube configuration of the Rubik's Cube." );
	wxMenuItem* saveCubeMenuItem = new wxMenuItem( programMenu, ID_SaveCube, "Save Cube", "Save the Rubik's Cube to file." );
	wxMenuItem* loadCubeMenuItem = new wxMenuItem( programMenu, ID_LoadCube, "Load Cube", "Load a Rubik's Cube from file." );
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit", "Exit this program." );
	programMenu->Append( createCubeMenuItem );
	programMenu->Append( destroyCubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( saveCubeMenuItem );
	programMenu->Append( loadCubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( scrambleCubeMenuItem );
	programMenu->Append( solveCubeMenuItem );
	programMenu->Append( cubeInACubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( exitMenuItem );

	wxMenu* rotationHistoryMenu = new wxMenu();
	wxMenuItem* rotationHistoryClearMenuItem = new wxMenuItem( rotationHistoryMenu, ID_RotationHistoryClear, "Clear History", "Delete all rotation history." );
	wxMenuItem* rotationHistoryGoForwardMenuItem = new wxMenuItem( rotationHistoryMenu, ID_RotationHistoryGoForward, "Go Forward\tF5", "Apply the rotation that takes us forward in the rotation history." );
	wxMenuItem* rotationHisotryGoBackwardMenuItem = new wxMenuItem( rotationHistoryMenu, ID_RotationHistoryGoBackward, "Go Backward\tF6", "Apply the rotation that takes us backward in the rotation history." );
	rotationHistoryMenu->Append( rotationHistoryGoForwardMenuItem );
	rotationHistoryMenu->Append( rotationHisotryGoBackwardMenuItem );
	rotationHistoryMenu->AppendSeparator();
	rotationHistoryMenu->Append( rotationHistoryClearMenuItem );

	wxMenu* viewMenu = new wxMenu();
	wxMenuItem* takeSnapShotMenuItem = new wxMenuItem( viewMenu, ID_TakeSnapShot, "Take Snap Shot", "Remember the Rubik's Cube as it is in its current state for comparison against a later state of the cube." );
	wxMenuItem* showInvariantFacesMenuItem = new wxMenuItem( viewMenu, ID_ShowInvariantFaces, "Show Invariant Faces", "Highlight all faces in common between the current Rubik's cube and a snap-shot, if any, taken earlier.", wxITEM_CHECK );
	wxMenuItem* showPerspectiveLabelsMenuItem = new wxMenuItem( viewMenu, ID_ShowPerspectiveLabels, "Show Perspective Labels", "Along with the cube, render labels of the cube faces that indicate the current perspective.", wxITEM_CHECK );
	wxMenuItem* renderWithPerspectiveProjectionMenuItem = new wxMenuItem( viewMenu, ID_RenderWithPerspectiveProjection, "Render With Perspective Projection", "Render the cube using an perspective projection.", wxITEM_CHECK );
	wxMenuItem* renderWithOrthographicProjectionMenuItem = new wxMenuItem( viewMenu, ID_RenderWithOrthographicProjection, "Render With Orthographic Projection", "Render the cube using an orthographic projection.", wxITEM_CHECK );
	viewMenu->Append( takeSnapShotMenuItem );
	viewMenu->AppendSeparator();
	viewMenu->Append( showInvariantFacesMenuItem );
	viewMenu->Append( showPerspectiveLabelsMenuItem );
	viewMenu->AppendSeparator();
	viewMenu->Append( renderWithPerspectiveProjectionMenuItem );
	viewMenu->Append( renderWithOrthographicProjectionMenuItem );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* debugModeMenuItem = new wxMenuItem( helpMenu, ID_DebugMode, "Debug Mode", "Continually scramble and solve the Rubik's Cube.", wxITEM_CHECK );
	wxMenuItem* silentDebugModeMenuItem = new wxMenuItem( helpMenu, ID_SilentDebugMode, "Silent Debug Mode", "Scramble and solve many Rubik's Cubes internally." );
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About\tF1", "Show the about-box." );
	helpMenu->Append( debugModeMenuItem );
	helpMenu->Append( silentDebugModeMenuItem );
	helpMenu->AppendSeparator();
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	menuBar->Append( viewMenu, "View" );
	menuBar->Append( rotationHistoryMenu, "Rotation History" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	canvas = new Canvas( this );

	textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_LEFT );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxALL | wxGROW, 0 );
	boxSizer->Add( textCtrl, 0, wxALL | wxGROW, 0 );
	SetSizer( boxSizer );

	wxAcceleratorEntry acceleratorEntries[3];
	acceleratorEntries[0].Set( wxACCEL_NORMAL, WXK_F5, ID_RotationHistoryGoBackward );
	acceleratorEntries[1].Set( wxACCEL_NORMAL, WXK_F6, ID_RotationHistoryGoForward );
	acceleratorEntries[2].Set( wxACCEL_NORMAL, WXK_F1, ID_About, aboutMenuItem );

	wxAcceleratorTable acceleratorTable( sizeof( acceleratorEntries ) / sizeof( wxAcceleratorEntry ), acceleratorEntries );
	SetAcceleratorTable( acceleratorTable );

	Bind( wxEVT_MENU, &Frame::OnCreateCube, this, ID_CreateCube );
	Bind( wxEVT_MENU, &Frame::OnDestroyCube, this, ID_DestroyCube );
	Bind( wxEVT_MENU, &Frame::OnScrambleCube, this, ID_ScrambleCube );
	Bind( wxEVT_MENU, &Frame::OnSolveCube, this, ID_SolveCube );
	Bind( wxEVT_MENU, &Frame::OnCubeInACube, this, ID_CubeInACube );
	Bind( wxEVT_MENU, &Frame::OnSaveCube, this, ID_SaveCube );
	Bind( wxEVT_MENU, &Frame::OnLoadCube, this, ID_LoadCube );
	Bind( wxEVT_MENU, &Frame::OnShowPerspectiveLabels, this, ID_ShowPerspectiveLabels );
	Bind( wxEVT_MENU, &Frame::OnShowInvariantFaces, this, ID_ShowInvariantFaces );
	Bind( wxEVT_MENU, &Frame::OnTakeSnapShot, this, ID_TakeSnapShot );
	Bind( wxEVT_MENU, &Frame::OnRenderWithPerspectiveProjection, this, ID_RenderWithPerspectiveProjection );
	Bind( wxEVT_MENU, &Frame::OnRenderWithOrthographicProjection, this, ID_RenderWithOrthographicProjection );
	Bind( wxEVT_MENU, &Frame::OnRotationHistoryGoForward, this, ID_RotationHistoryGoForward );
	Bind( wxEVT_MENU, &Frame::OnRotationHistoryGoBackward, this, ID_RotationHistoryGoBackward );
	Bind( wxEVT_MENU, &Frame::OnRotationHistoryClear, this, ID_RotationHistoryClear );
	Bind( wxEVT_MENU, &Frame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &Frame::OnDebugMode, this, ID_DebugMode );
	Bind( wxEVT_MENU, &Frame::OnSilentDebugMode, this, ID_SilentDebugMode );
	Bind( wxEVT_MENU, &Frame::OnAbout, this, ID_About );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_CreateCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_DestroyCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_ScrambleCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_SolveCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_CubeInACube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_SaveCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_LoadCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_ShowPerspectiveLabels );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_ShowInvariantFaces );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_TakeSnapShot );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_RenderWithPerspectiveProjection );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_RenderWithOrthographicProjection );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_RotationHistoryGoForward );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_RotationHistoryGoBackward );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_RotationHistoryClear );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_DebugMode );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_SilentDebugMode );
	Bind( wxEVT_TIMER, &Frame::OnTimer, this, ID_Timer );
	Bind( wxEVT_COMMAND_TEXT_ENTER, &Frame::OnTextCtrlEnter, this );

	timer.Start( 1 );
}

//==================================================================================================
/*virtual*/ Frame::~Frame( void )
{
}

//==================================================================================================
void Frame::OnDebugMode( wxCommandEvent& event )
{
	if( debugMode == DEBUG_MODE_NONE )
		debugMode = DEBUG_MODE_SCRAMBLE;
	else
		debugMode = DEBUG_MODE_NONE;
}

//==================================================================================================
void Frame::OnSilentDebugMode( wxCommandEvent& event )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return;

	int solveCount = ( int )wxGetNumberFromUser( "How many Rubik's Cubes should the program solve?", "How many:", "Number of cubes to solve", 100, 1, 1000000 );
	for( int count = 0; count < solveCount; count++ )
	{
		rubiksCube->Scramble( time(0), 100, 0, true );
		
		bool success = rubiksCube->SaveToFile( "debugCube.xml" );
		wxASSERT( success );

		Solver* solver = rubiksCube->MakeSolver();
		wxASSERT( solver );
		if( !solver )
			break;

		RubiksCube::RotationSequence rotationSequence;
		success = solver->MakeEntireSolutionSequence( rubiksCube, rotationSequence );
		wxASSERT( success );

		delete solver;

		success = rubiksCube->ApplySequence( rotationSequence );
		wxASSERT( success );

		success = rubiksCube->IsInSolvedState();
		wxASSERT( success );
	}
}

//==================================================================================================
void Frame::OnSaveCube( wxCommandEvent& event )
{
	if( !wxGetApp().rubiksCube )
		return;

	wxFileDialog fileDialog( this, "Save Rubik's Cube File", wxEmptyString, wxEmptyString, "Rubik's Cube Files (*.xml)|*.xml", wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
	if( wxID_OK != fileDialog.ShowModal() )
		return;

	wxString file = fileDialog.GetPath();
	if( !wxGetApp().rubiksCube->SaveToFile( file ) )
		wxMessageBox( "Failed to save file \"" + file + "\".", "Save Failure" );
}

//==================================================================================================
void Frame::OnLoadCube( wxCommandEvent& event )
{
	if( wxGetApp().rubiksCube )
		return;

	wxFileDialog fileDialog( this, "Load Rubik's Cube File", wxEmptyString, wxEmptyString, "Rubik's Cube Files (*.xml)|*.xml", wxFD_OPEN | wxFD_FILE_MUST_EXIST );
	if( wxID_OK != fileDialog.ShowModal() )
		return;

	wxString file = fileDialog.GetPath();
	wxGetApp().rubiksCube = RubiksCube::LoadFromFile( file );
	if( !wxGetApp().rubiksCube )
		wxMessageBox( "Failed to load file \"" + file + "\".", "Load Failure" );
	else
	{
		canvas->AdjustSizeFor( wxGetApp().rubiksCube );
		canvas->Refresh();
		wxGetApp().RotationHistoryClear();
	}
}

//==================================================================================================
void Frame::OnRenderWithPerspectiveProjection( wxCommandEvent& event )
{
	canvas->SetProjection( Canvas::PERSPECTIVE );
	canvas->Refresh();
}

//==================================================================================================
void Frame::OnRenderWithOrthographicProjection( wxCommandEvent& event )
{
	canvas->SetProjection( Canvas::ORTHOGRAPHIC );
	canvas->Refresh();
}

//==================================================================================================
void Frame::OnShowPerspectiveLabels( wxCommandEvent& event )
{
	canvas->ShowPerspectiveLabels( !canvas->ShowPerspectiveLabels() );
	canvas->Refresh();
}

//==================================================================================================
void Frame::OnShowInvariantFaces( wxCommandEvent& event )
{
	canvas->ShowInvariantFaces( !canvas->ShowInvariantFaces() );
	canvas->Refresh();
}

//==================================================================================================
void Frame::OnTakeSnapShot( wxCommandEvent& event )
{
	if( canvas->TakeSnapShot() )
	{
		wxStatusBar* statusBar = GetStatusBar();
		statusBar->SetStatusText( "Snap-shot taken!" );
		canvas->Refresh();
	}
}

//==================================================================================================
void Frame::OnTextCtrlEnter( wxCommandEvent& event )
{
	if( !wxGetApp().rubiksCube )
		return;

	if( executionSequence.size() > 0 || canvas->IsAnimating( 1.0 ) )
		return;

	wxString relativeRotationSequenceString = textCtrl->GetValue();

	RubiksCube::RelativeRotationSequence relativeRotationSequence;
	if( ParseRelativeRotationSequenceString( relativeRotationSequenceString, relativeRotationSequence ) )
	{
		RubiksCube::Perspective perspective;
		canvas->DeterminePerspective( perspective );

		if( !wxGetApp().rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, executionSequence ) )
			executionSequence.clear();
		else
		{
			wxGetApp().rubiksCube->CompressRotationSequence( executionSequence );
			animationTolerance = 0.01;
		}
	}
}

//==================================================================================================
/*static*/ bool Frame::ParseRelativeRotationSequenceString( const wxString& relativeRotationSequenceString, RubiksCube::RelativeRotationSequence& relativeRotationSequence )
{
	std::string string = ( const char* )relativeRotationSequenceString.c_str();
	if( !wxGetApp().rubiksCube->ParseRelativeRotationSequenceString( string, relativeRotationSequence ) )
	{
		wxMessageBox( "Failed to parse string \"" + relativeRotationSequenceString + "\".", "Parse Error" );
		return false;
	}
	return true;
}

//==================================================================================================
void Frame::OnTimer( wxTimerEvent& event )
{
	if( !canvas->IsAnimating( animationTolerance ) )
	{
		wxStatusBar* statusBar = GetStatusBar();

		if( executionSequence.size() > 0 )
		{
			RubiksCube::RotationSequence::iterator iter = executionSequence.begin();
			RubiksCube::Rotation rotation = *iter;
			executionSequence.erase( iter );
			canvas->ApplyRotation( rotation );

			statusBar->SetStatusText( wxString::Format( "%d animation moves remaining.", executionSequence.size() ) );
		}
		else
		{
			statusBar->SetStatusText( "" );

			if( debugMode != DEBUG_MODE_NONE && wxGetApp().rubiksCube )
			{
				RubiksCube* rubiksCube = wxGetApp().rubiksCube;
				switch( debugMode )
				{
					case DEBUG_MODE_SCRAMBLE:
					{
						// This message doesn't mean anything if we entered debug mode with an unsolved cube.
						bool solved = rubiksCube->IsInSolvedState();
						if( !solved )
							wxMessageBox( "Failed to solve cube!", "Error", wxCENTRE | wxICON_ERROR );
						rubiksCube->Scramble( time(0), 100, &executionSequence, false );
						debugMode = DEBUG_MODE_SOLVE;
						animationTolerance = 2.0;
						break;
					}
					case DEBUG_MODE_SOLVE:
					{
						Solver* solver = rubiksCube->MakeSolver();
						if( solver )
						{
							bool success = rubiksCube->SaveToFile( "debugCube.xml" );
							wxASSERT( success );
							success = solver->MakeEntireSolutionSequence( wxGetApp().rubiksCube, executionSequence );
							wxASSERT( success );
							debugMode = DEBUG_MODE_SCRAMBLE;
							animationTolerance = 2.0;
							delete solver;
						}
						break;
					}
				}
			}
		}
	}

	if( canvas->Animate() )
		canvas->Refresh();
}

//==================================================================================================
void Frame::OnCreateCube( wxCommandEvent& event )
{
	if( wxGetApp().rubiksCube )
		return;
	
	int subCubeMatrixSize = ( int )wxGetNumberFromUser( "Enter what will be the width, height and depth of the cube.", "Size (2-12):", "Rubik's Cube Size", 3, 2, 12 );
	if( subCubeMatrixSize < 0 )
		return;

	wxGetApp().rubiksCube = new RubiksCube( subCubeMatrixSize );
	canvas->AdjustSizeFor( wxGetApp().rubiksCube );
	canvas->Refresh();

	wxGetApp().RotationHistoryClear();
}

//==================================================================================================
void Frame::OnDestroyCube( wxCommandEvent& event )
{
	if( !wxGetApp().rubiksCube )
		return;

	delete wxGetApp().rubiksCube;
	wxGetApp().rubiksCube = 0;
	canvas->Refresh();

	wxGetApp().RotationHistoryClear();
}

//==================================================================================================
void Frame::OnScrambleCube( wxCommandEvent& event )
{
	if( executionSequence.size() != 0 )
		return;

	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return;

	rubiksCube->Scramble( time(0), 40, &executionSequence, false );
	animationTolerance = 1.0;
}

//==================================================================================================
void Frame::OnSolveCube( wxCommandEvent& event )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return;

	Solver* solver = rubiksCube->MakeSolver();
	if( !solver )
		return;

	if( !solver->MakeEntireSolutionSequence( rubiksCube, executionSequence ) )
	{
		executionSequence.clear();
		wxMessageBox( "An internal error occurred while attempting to solve the cube.", "Error" );
	}

	delete solver;

	animationTolerance = 1.0; //0.01;
}

//==================================================================================================
void Frame::OnCubeInACube( wxCommandEvent& event )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return;

	CubeInACube cubeInACube;

	if( !cubeInACube.MakeEntireSolutionSequence( rubiksCube, executionSequence ) )
	{
		executionSequence.clear();
		wxMessageBox( "An error occurred while attempting to solve for cube-in-a-cube.", "Error" );
	}

	animationTolerance = 1.0;
}

//==================================================================================================
void Frame::OnRotationHistoryClear( wxCommandEvent& event )
{
	wxGetApp().RotationHistoryClear();
}

//==================================================================================================
void Frame::OnRotationHistoryGoForward( wxCommandEvent& event )
{
	RubiksCube::Rotation rotation;
	if( wxGetApp().RotationHistoryGoForward( rotation ) )
		canvas->ApplyRotation( rotation, false );
}

//==================================================================================================
void Frame::OnRotationHistoryGoBackward( wxCommandEvent& event )
{
	RubiksCube::Rotation rotation;
	if( wxGetApp().RotationHistoryGoBackward( rotation ) )
		canvas->ApplyRotation( rotation, false );
}

//==================================================================================================
void Frame::OnExit( wxCommandEvent& event )
{
	timer.Stop();

	Close( true );
}

//==================================================================================================
void Frame::OnAbout( wxCommandEvent& event )
{
	wxAboutDialogInfo aboutDialogInfo;
        
    aboutDialogInfo.SetName( "Rubik's Cube" );
    aboutDialogInfo.SetVersion( "1.0" );
    aboutDialogInfo.SetDescription( "This program is free software and distributed under the MIT license." );
    aboutDialogInfo.SetCopyright( "Copyright (C) 2013, 2015 -- Spencer T. Parkin <SpencerTParkin@gmail.com>" );

    wxAboutBox( aboutDialogInfo );
}

//==================================================================================================
void Frame::OnUpdateMenuItemUI( wxUpdateUIEvent& event )
{
	switch( event.GetId() )
	{
		case ID_CreateCube:
		case ID_LoadCube:
		{
			event.Enable( wxGetApp().rubiksCube == 0 ? true : false );
			break;
		}
		case ID_DestroyCube:
		case ID_SaveCube:
		case ID_TakeSnapShot:
		case ID_SilentDebugMode:
		{
			event.Enable( wxGetApp().rubiksCube != 0 ? true : false );
			break;
		}
		case ID_CubeInACube:
		{
			event.Enable( wxGetApp().rubiksCube != 0 ? wxGetApp().rubiksCube->IsInSolvedState() : false );
			break;
		}
		case ID_SolveCube:
		case ID_ScrambleCube:
		{
			event.Enable( ( wxGetApp().rubiksCube != 0 && executionSequence.size() == 0 ) ? true : false );
			break;
		}
		case ID_RenderWithPerspectiveProjection:
		{
			event.Check( canvas->GetProjection() == Canvas::PERSPECTIVE ? true : false );
			break;
		}
		case ID_RenderWithOrthographicProjection:
		{
			event.Check( canvas->GetProjection() == Canvas::ORTHOGRAPHIC ? true : false );
			break;
		}
		case ID_ShowPerspectiveLabels:
		{
			event.Check( canvas->ShowPerspectiveLabels() );
			break;
		}
		case ID_ShowInvariantFaces:
		{
			event.Check( canvas->ShowInvariantFaces() );
			break;
		}
		case ID_RotationHistoryGoForward:
		{
			event.Enable( wxGetApp().RotationHistoryCanGoForward() );
			break;
		}
		case ID_RotationHistoryGoBackward:
		{
			event.Enable( wxGetApp().RotationHistoryCanGoBackward() );
			break;
		}
		case ID_RotationHistoryClear:
		{
			event.Enable( wxGetApp().RotationHistoryCanGoForward() || wxGetApp().RotationHistoryCanGoBackward() );
			break;
		}
		case ID_DebugMode:
		{
			event.Check( ( debugMode == DEBUG_MODE_NONE ) ? false : true );
			break;
		}
	}
}

// Frame.cpp