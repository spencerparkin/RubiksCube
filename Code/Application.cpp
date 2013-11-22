// Application.cpp

#include "Header.h"

//==================================================================================================
wxIMPLEMENT_APP( Application );

//==================================================================================================
Application::Application( void )
{
	rubiksCube = 0;
	solver = 0;
}

//==================================================================================================
/*virtual*/ Application::~Application( void )
{
	delete rubiksCube;
	delete solver;
}

//==================================================================================================
/*virtual*/ bool Application::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	Frame* frame = new Frame( 0, wxDefaultPosition, wxSize( 500, 500 ) );
	frame->Show();

	return true;
}

//==================================================================================================
/*virtual*/ int Application::OnExit( void )
{
	return 0;
}

// Application.cpp