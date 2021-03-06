// Application.cpp

#include "Header.h"

//==================================================================================================
wxIMPLEMENT_APP( Application );

//==================================================================================================
Application::Application( void )
{
	rubiksCube = 0;
	rotationHistoryIndex = -1;
	makeInitialCube = true;
	frame = nullptr;
}

//==================================================================================================
/*virtual*/ Application::~Application( void )
{
	SetRubiksCube( nullptr );
}

//==================================================================================================
/*virtual*/ bool Application::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	wxInitAllImageHandlers();

	// There is never a solution to the following scenerio.  Can this happen in the 3x3x3 case?
	// I was able to get it to happen in the 4x4x4 case.  Using tri-cycles, one can shift the
	// entire set of four forward or backward, as a whole, with wrapping, by multiples of 2 only.
	// It follows that if the two quads are mis-aligned by an odd number of rotations, they cannot
	// come into alignment using just tri-cycles.  If there was a way to swap two opposite edges or
	// corners, then it may be possible to finish with tri-cycles.
	//
	// To answer this question, yes, there is a way.  Chris Hardwick gives the sequence
	// {2r, 2U, 2r, 2U, 2u, 2r, 2u}, which will swap to opposite edge peices.  Tri-cycles
	// can then be used to solve the final layer.  Also, I don't believe this misalignment
	// can happen in the 3x3x3 case.  It can, however, in the 4x4x4 case.
	TriCycleSolver::TriCycleSequence triCycleSequence;
	int quadCorners[4] = { 1, 2, 3, 0 };	// Corners
	int quadEdges[4] = { 0, 1, 2, 3 };		// Edges
	bool result = TriCycleSolver::FindSmallestTriCycleSequenceThatOrdersQuadTheSame( quadCorners, quadEdges, triCycleSequence );

	frame = new Frame( 0, wxDefaultPosition, wxSize( 500, 500 ) );
	frame->Show();

	return true;
}

//==================================================================================================
/*virtual*/ int Application::OnExit( void )
{
	return 0;
}

//==================================================================================================
void Application::SetRubiksCube( RubiksCube* rubiksCube )
{
	if( this->rubiksCube )
	{
		delete this->rubiksCube;
		this->rubiksCube = nullptr;
	}

	this->rubiksCube = rubiksCube;
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
wxImage* Application::LoadTextureResource( const wxString& textureFile )
{
	wxImage* image = new wxImage();
	
	//image->SetLoadFlags( image->GetLoadFlags() & ~wxImage::Load_Verbose );
	
	wxArrayString fileLocationsArray;
	fileLocationsArray.Add( wxString( wxGetenv( "SNAP" ) ) + "/share/RubiksCube/Textures/" + textureFile );
	fileLocationsArray.Add( wxString( "../share/RubiksCube/Textures/" ) + textureFile  );
	fileLocationsArray.Add( wxString( "Textures/" ) + textureFile );
	
	int i;
	for( i = 0; i < ( int )fileLocationsArray.GetCount(); i++ )
	{
		wxFileName fileName( fileLocationsArray[i] );
		if( fileName.FileExists() && image->LoadFile( fileName.GetFullPath(), wxBITMAP_TYPE_PNG ) )
			break;
	}
	
	if( i == fileLocationsArray.GetCount() )
	{
		delete image;
		image = nullptr;
	}
	
	return image;
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
