// Frame.cpp

#include "Header.h"

//==================================================================================================
Frame::Frame( wxWindow* parent, const wxPoint& pos, const wxSize& size ) : wxFrame( parent, wxID_ANY, "Rubik's Cube", pos, size ), timer( this, ID_Timer )
{
	wxMenu* programMenu = new wxMenu();
	wxMenuItem* createCubeMenuItem = new wxMenuItem( programMenu, ID_CreateCube, "Create Cube\tF5", "Create a new Rubik's Cube." );
	wxMenuItem* destroyCubeMenuItem = new wxMenuItem( programMenu, ID_DestroyCube, "Destroy Cube\tF6", "Destroy the Rubik's Cube." );
	wxMenuItem* scrambleCubeMenuItem = new wxMenuItem( programMenu, ID_ScrambleCube, "Scramble Cube\tF12", "Randomize the Rubik's Cube." );
	wxMenuItem* solveCubeMenuItem = new wxMenuItem( programMenu, ID_SolveCube, "Solve Cube", "Solve the Rubik's Cube." );
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit", "Exit this program." );
	programMenu->Append( createCubeMenuItem );
	programMenu->Append( destroyCubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( scrambleCubeMenuItem );
	programMenu->Append( solveCubeMenuItem );
	//programMenu->AppendSeparator();
	//programMenu->Append( saveCubeMenuItem );
	//programMenu->Append( loadCubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( exitMenuItem );

	wxMenu* moveStackMenu = new wxMenu();
	wxMenuItem* popMoveAndApplyMenuItem = new wxMenuItem( programMenu, ID_PopMoveAndApply, "Pop Move and Apply\tF9", "Pop and apply the move on the top of the move-stack." );
	wxMenuItem* popMoveAndNoApplyMenuItem = new wxMenuItem( programMenu, ID_PopMoveAndNoApply, "Pop Move and No Apply\tShift+F9", "Pop, but do not apply, the move on the top of the move-stack." );
	moveStackMenu->Append( popMoveAndApplyMenuItem );
	moveStackMenu->Append( popMoveAndNoApplyMenuItem );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About\tF1", "Show the about-box." );
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	menuBar->Append( moveStackMenu, "Move Stack" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	Bind( wxEVT_MENU, &Frame::OnCreateCube, this, ID_CreateCube );
	Bind( wxEVT_MENU, &Frame::OnDestroyCube, this, ID_DestroyCube );
	Bind( wxEVT_MENU, &Frame::OnScrambleCube, this, ID_ScrambleCube );
	Bind( wxEVT_MENU, &Frame::OnSolveCube, this, ID_SolveCube );
	Bind( wxEVT_MENU, &Frame::OnPopMoveAndApply, this, ID_PopMoveAndApply );
	Bind( wxEVT_MENU, &Frame::OnPopMoveAndNoApply, this, ID_PopMoveAndNoApply );
	Bind( wxEVT_MENU, &Frame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &Frame::OnAbout, this, ID_About );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_CreateCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_DestroyCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_ScrambleCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_SolveCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_PopMoveAndApply );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_PopMoveAndNoApply );
	Bind( wxEVT_TIMER, &Frame::OnTimer, this, ID_Timer );

	canvas = new Canvas( this );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxALL | wxGROW, 0 );
	SetSizer( boxSizer );

	wxAcceleratorEntry acceleratorEntries[6];
	acceleratorEntries[0].Set( wxACCEL_NORMAL, WXK_F5, ID_CreateCube, createCubeMenuItem );
	acceleratorEntries[1].Set( wxACCEL_NORMAL, WXK_F6, ID_DestroyCube, destroyCubeMenuItem );
	acceleratorEntries[2].Set( wxACCEL_NORMAL, WXK_F12, ID_ScrambleCube, scrambleCubeMenuItem );
	acceleratorEntries[3].Set( wxACCEL_NORMAL, WXK_F9, ID_PopMoveAndApply, popMoveAndApplyMenuItem );
	acceleratorEntries[4].Set( wxACCEL_SHIFT, WXK_F9, ID_PopMoveAndNoApply, popMoveAndNoApplyMenuItem );
	acceleratorEntries[5].Set( wxACCEL_NORMAL, WXK_F1, ID_About, aboutMenuItem );

	wxAcceleratorTable acceleratorTable( sizeof( acceleratorEntries ) / sizeof( wxAcceleratorEntry ), acceleratorEntries );
	SetAcceleratorTable( acceleratorTable );

	timer.Start( 1 );
}

//==================================================================================================
/*virtual*/ Frame::~Frame( void )
{
}

//==================================================================================================
void Frame::OnTimer( wxTimerEvent& event )
{
	if( !canvas->IsAnimating( 1.0 ) )
	{
		Solver* solver = wxGetApp().solver;
		if( solver )
		{
			RubiksCube::Rotation rotation;
			if( !solver->MakeMove( rotation ) || !canvas->ApplyRotation( rotation ) )
			{
				delete solver;
				wxGetApp().solver = 0;
			}
		}
		else if( scrambleList.size() > 0 )
		{
			RubiksCube::Rotation rotation = scrambleList.back();
			scrambleList.pop_back();
			canvas->ApplyRotation( rotation );
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
	
	int subCubeMatrixSize = ( int )wxGetNumberFromUser( "Enter what will be the width, height and depth of the cube.", "Size (2-6):", "Rubik's Cube Size", 3, 2, 6 );
	if( subCubeMatrixSize < 0 )
		return;

	int colorCount = ( int )wxGetNumberFromUser( "Enter the number of face colors for the cube in the solved state.", "Colors (1-6):", "Rubik's Cube Color Count", 6, 1, 6 );
	if( colorCount < 0 )
		return;

	wxGetApp().rubiksCube = new RubiksCube( subCubeMatrixSize, colorCount );
	canvas->AdjustSizeFor( wxGetApp().rubiksCube );
	canvas->Refresh();

	wxGetApp().moveStack.clear();
}

//==================================================================================================
void Frame::OnDestroyCube( wxCommandEvent& event )
{
	if( !wxGetApp().rubiksCube )
		return;

	delete wxGetApp().rubiksCube;
	wxGetApp().rubiksCube = 0;
	canvas->Refresh();

	wxGetApp().moveStack.clear();
}

//==================================================================================================
void Frame::OnScrambleCube( wxCommandEvent& event )
{
	if( scrambleList.size() != 0 )
		return;

	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return;

	srand( unsigned( time( 0 ) ) );
	for( int i = 0; i < 50; i++ )
	{
		RubiksCube::Rotation rotation;
		rotation.plane.index = rand() % rubiksCube->SubCubeMatrixSize();
		rotation.plane.axis = RubiksCube::Axis( rand() % 3 );
		rotation.angle = double( 1 + rand() % 3 ) * M_PI / 2.0;
		scrambleList.push_back( rotation );
	}
}

//==================================================================================================
void Frame::OnSolveCube( wxCommandEvent& event )
{
	if( wxGetApp().solver )
		return;

	wxGetApp().solver = new Solver();
}

//==================================================================================================
void Frame::OnPopMoveAndApply( wxCommandEvent& event )
{
	if( wxGetApp().moveStack.size() == 0 )
		return;

	RubiksCube::Rotation rotation = wxGetApp().moveStack.back();
	wxGetApp().moveStack.pop_back();
	canvas->ApplyRotation( rotation );
}

//==================================================================================================
void Frame::OnPopMoveAndNoApply( wxCommandEvent& event )
{
	if( wxGetApp().moveStack.size() > 0 )
		wxGetApp().moveStack.pop_back();
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
    aboutDialogInfo.SetCopyright( "Copyright (C) 2013 Spencer T. Parkin <SpencerTParkin@gmail.com>" );

    wxAboutBox( aboutDialogInfo );
}

//==================================================================================================
void Frame::OnUpdateMenuItemUI( wxUpdateUIEvent& event )
{
	switch( event.GetId() )
	{
		case ID_CreateCube:
		{
			event.Enable( wxGetApp().rubiksCube == 0 ? true : false );
			break;
		}
		case ID_DestroyCube:
		case ID_ScrambleCube:
		{
			event.Enable( wxGetApp().rubiksCube == 0 ? false : true );
			break;
		}
		case ID_SolveCube:
		{
			event.Enable( wxGetApp().rubiksCube != 0 && wxGetApp().solver == 0 ? true : false );
			break;
		}
		case ID_PopMoveAndApply:
		case ID_PopMoveAndNoApply:
		{
			event.Enable( ( wxGetApp().rubiksCube != 0 && wxGetApp().moveStack.size() > 0 ) ? true : false );
			break;
		}
	}
}

// Frame.cpp