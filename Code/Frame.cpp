// Frame.cpp

#include "Header.h"

//==================================================================================================
Frame::Frame( wxWindow* parent ) : wxFrame( parent, wxID_ANY, "Rubik's Cube" ), timer( this, ID_Timer )
{
	wxMenu* programMenu = new wxMenu();
	wxMenuItem* createCubeMenuItem = new wxMenuItem( programMenu, ID_CreateCube, "Create Cube\tF5", "Create a new Rubik's Cube." );
	wxMenuItem* destroyCubeMenuItem = new wxMenuItem( programMenu, ID_DestroyCube, "Destroy Cube\tF6", "Destroy the Rubik's Cube." );
	wxMenuItem* popMoveMenuItem = new wxMenuItem( programMenu, ID_PopMove, "Pop Move\tF9", "Pop and apply the move on the top of the move-stack." );
	wxMenuItem* scrambleCubeMenuItem = new wxMenuItem( programMenu, ID_ScrambleCube, "Scramble Cube\tF12", "Randomize the Rubik's Cube." );
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit", "Exit this program." );
	programMenu->Append( createCubeMenuItem );
	programMenu->Append( destroyCubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( popMoveMenuItem );
	programMenu->Append( scrambleCubeMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( exitMenuItem );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About\tF1", "Show the about-box." );
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	Bind( wxEVT_MENU, &Frame::OnCreateCube, this, ID_CreateCube );
	Bind( wxEVT_MENU, &Frame::OnDestroyCube, this, ID_DestroyCube );
	Bind( wxEVT_MENU, &Frame::OnScrambleCube, this, ID_ScrambleCube );
	Bind( wxEVT_MENU, &Frame::OnPopMove, this, ID_PopMove );
	Bind( wxEVT_MENU, &Frame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &Frame::OnAbout, this, ID_About );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_CreateCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_DestroyCube );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_PopMove );
	Bind( wxEVT_UPDATE_UI, &Frame::OnUpdateMenuItemUI, this, ID_ScrambleCube );
	Bind( wxEVT_TIMER, &Frame::OnTimer, this, ID_Timer );

	canvas = new Canvas( this );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxALL | wxGROW, 0 );
	SetSizer( boxSizer );

	wxAcceleratorEntry acceleratorEntries[5];
	acceleratorEntries[0].Set( wxACCEL_NORMAL, WXK_F5, ID_CreateCube, createCubeMenuItem );
	acceleratorEntries[1].Set( wxACCEL_NORMAL, WXK_F6, ID_DestroyCube, destroyCubeMenuItem );
	acceleratorEntries[2].Set( wxACCEL_NORMAL, WXK_F9, ID_PopMove, popMoveMenuItem );
	acceleratorEntries[3].Set( wxACCEL_NORMAL, WXK_F1, ID_About, aboutMenuItem );
	acceleratorEntries[4].Set( wxACCEL_NORMAL, WXK_F12, ID_ScrambleCube, scrambleCubeMenuItem );

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
	if( scrambleList.size() > 0 && !canvas->IsAnimating( 1.0 ) )
	{
		RubiksCube::Rotation rotation = scrambleList.back();
		scrambleList.pop_back();
		canvas->ApplyRotation( rotation );
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

	wxGetApp().rubiksCube = new RubiksCube( subCubeMatrixSize );
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
void Frame::OnPopMove( wxCommandEvent& event )
{
	if( wxGetApp().moveStack.size() == 0 )
		return;

	RubiksCube::Rotation rotation = wxGetApp().moveStack.back();
	wxGetApp().moveStack.pop_back();
	canvas->ApplyRotation( rotation );
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
		case ID_PopMove:
		{
			event.Enable( ( wxGetApp().rubiksCube != 0 && wxGetApp().moveStack.size() > 0 ) ? true : false );
			break;
		}
	}
}

// Frame.cpp