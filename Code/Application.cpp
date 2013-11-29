// Application.cpp

#include "Header.h"

//==================================================================================================
wxIMPLEMENT_APP( Application );

//==================================================================================================
Application::Application( void )
{
	rubiksCube = 0;
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
void Application::PushRotation( const RubiksCube::Rotation& rotation )
{
	if( rotationStack.size() == ROTATION_STACK_CAPACITY )
		rotationStack.pop_front();

	rotationStack.push_back( rotation );
}

//==================================================================================================
bool Application::PopRotation( RubiksCube::Rotation* rotation /*= 0*/ )
{
	if( rotationStack.size() == 0 )
		return false;

	if( rotation )
		*rotation = rotationStack.back();

	rotationStack.pop_back();
	return true;
}

// Application.cpp