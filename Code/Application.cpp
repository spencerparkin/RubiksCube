// Application.cpp

#include "Header.h"

//==================================================================================================
wxIMPLEMENT_APP( Application );

//==================================================================================================
Application::Application( void )
{
	rubiksCube = 0;
	rotationHistoryIndex = -1;
}

//==================================================================================================
/*virtual*/ Application::~Application( void )
{
	delete rubiksCube;
}

//==================================================================================================
/*virtual*/ bool Application::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	wxInitAllImageHandlers();

	Frame* frame = new Frame( 0, wxDefaultPosition, wxSize( 500, 500 ) );
	frame->Show();

	return true;
}

//==================================================================================================
/*virtual*/ int Application::OnExit( void )
{
	return 0;
}

//==================================================================================================
void Application::RotationHistoryAppend( const RubiksCube::Rotation& rotation )
{
	// We always append to the history at our current location, which means
	// forfeiting any rotations that are in our future.
	if( rotationHistoryIndex != -1 )
		rotationHistory.resize( rotationHistoryIndex );
	else
		rotationHistoryIndex = 0;

	rotationHistory.push_back( rotation );
	rotationHistoryIndex++;
}

//==================================================================================================
bool Application::RotationHistoryGoForward( RubiksCube::Rotation& rotation )
{
	if( !RotationHistoryCanGoForward() )
		return false;

	rotation = rotationHistory[ rotationHistoryIndex++ ];
	rotation.angle *= -1.0;
	return true;
}

//==================================================================================================
bool Application::RotationHistoryGoBackward( RubiksCube::Rotation& rotation )
{
	if( !RotationHistoryCanGoBackward() )
		return false;

	rotation = rotationHistory[ --rotationHistoryIndex ];
	return true;
}

//==================================================================================================
bool Application::RotationHistoryCanGoForward( void )
{
	if( rotationHistoryIndex == -1 || rotationHistoryIndex == rotationHistory.size() )
		return false;
	return true;
}

//==================================================================================================
bool Application::RotationHistoryCanGoBackward( void )
{
	if( rotationHistoryIndex == -1 || rotationHistoryIndex == 0 )
		return false;
	return true;
}

//==================================================================================================
void Application::RotationHistoryClear( void )
{
	rotationHistory.clear();
	rotationHistoryIndex = -1;
}

// Application.cpp