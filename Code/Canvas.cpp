// Canvas.cpp

#include "Header.h"

// Note that on Linux, if the depth buffer size is not given, we don't get a depth buffer at all!
int Canvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0 };

//==================================================================================================
Canvas::Canvas( wxWindow* parent ) : wxGLCanvas( parent, wxID_ANY, attributeList )
{
	context = 0;

	Bind( wxEVT_PAINT, &Canvas::OnPaint, this );
	Bind( wxEVT_SIZE, &Canvas::OnSize, this );
	Bind( wxEVT_LEFT_DOWN, &Canvas::OnMouseLeftDown, this );
	Bind( wxEVT_RIGHT_DOWN, &Canvas::OnMouseRightDown, this );
	Bind( wxEVT_RIGHT_UP, &Canvas::OnMouseRightUp, this );
	Bind( wxEVT_MOTION, &Canvas::OnMouseMotion, this );
	Bind( wxEVT_MOUSE_CAPTURE_LOST, &Canvas::OnMouseCaptureLost, this );

	hitBuffer = 0;
	hitBufferSize = 0;

	camera.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 10.0, 0.0, 10.0 );
	camera.xAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, sqrt( 2.0 ) / 2.0, 0.0, -sqrt( 2.0 ) / 2.0 );
	camera.yAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );

	mousePos.x = 0;
	mousePos.y = 0;

	rotation.plane.axis = RubiksCube::Z_AXIS;
	rotation.plane.index = 0;
	rotation.angle = 0.0;

	size.cubeWidthHeightAndDepth = 5.0;
	size.subCubeWidthHeightAndDepth = 0.5;

	grip.plane[0].axis = RubiksCube::X_AXIS;
	grip.plane[0].index = 0;
	grip.plane[1].axis = RubiksCube::Y_AXIS;
	grip.plane[1].index = 0;

	grippingCube = false;
	projection = PERSPECTIVE;
	showPerspectiveLabels = false;
	showInvariantFaces = false;
	selectedFaceId = -1;
	comparativeRubiksCube = 0;
}

//==================================================================================================
/*virtual*/ Canvas::~Canvas( void )
{
	delete context;
	delete comparativeRubiksCube;
	DeletePerspectiveLabelMap();
}

//==================================================================================================
void Canvas::SetProjection( Projection projection )
{
	this->projection = projection;
}

//==================================================================================================
Canvas::Projection Canvas::GetProjection( void ) const
{
	return projection;
}

//==================================================================================================
void Canvas::ShowPerspectiveLabels( bool showPerspectiveLabels )
{
	this->showPerspectiveLabels = showPerspectiveLabels;
}

//==================================================================================================
bool Canvas::ShowPerspectiveLabels( void ) const
{
	return showPerspectiveLabels;
}

//==================================================================================================
void Canvas::ShowInvariantFaces( bool showInvariantFaces )
{
	this->showInvariantFaces = showInvariantFaces;
}

//==================================================================================================
bool Canvas::ShowInvariantFaces( void ) const
{
	return showInvariantFaces;
}

//==================================================================================================
void Canvas::AdjustSizeFor( const RubiksCube* rubiksCube )
{
	int subCubeMatrixSize = rubiksCube->SubCubeMatrixSize();
	size.cubeWidthHeightAndDepth = 5.0;
	size.subCubeWidthHeightAndDepth = size.cubeWidthHeightAndDepth / double( subCubeMatrixSize - 1 );
}

//==================================================================================================
bool Canvas::Animate( void )
{
	if( grippingCube )
		return false;

	if( !IsAnimating( 1e-8 ) )
	{
		rotation.angle = 0.0;
		return false;
	}

	rotation.angle *= 0.9;
	return true;
}

//==================================================================================================
bool Canvas::IsAnimating( double tolerance /*= 1e-7*/ ) const
{
	if( fabs( rotation.angle ) >= tolerance )
		return true;
	return false;
}

//==================================================================================================
bool Canvas::ApplyRotation( const RubiksCube::Rotation& rotation, bool appendToRotationHistory /*= true*/ )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return false;

	RubiksCube::Rotation reverseRotation;
	if( !rubiksCube->Apply( rotation, &this->rotation, &reverseRotation ) )
		return false;

	if( appendToRotationHistory )
		wxGetApp().RotationHistoryAppend( reverseRotation );

	return true;
}

//==================================================================================================
bool Canvas::TakeSnapShot( void )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return false;

	delete comparativeRubiksCube;
	comparativeRubiksCube = new RubiksCube( rubiksCube->SubCubeMatrixSize(), false );
	comparativeRubiksCube->Copy( *rubiksCube, RubiksCube::CopyMap );
	return true;
}

//==================================================================================================
void Canvas::BindContext( void )
{
	if( !context )
		context = new wxGLContext( this );

	SetCurrent( *context );
}

//==================================================================================================
void Canvas::PreRender( GLenum mode )
{
	BindContext();

	if( mode == GL_SELECT )
	{
		hitBufferSize = 512;
		hitBuffer = new unsigned int[ hitBufferSize ];
		glSelectBuffer( hitBufferSize, hitBuffer );
		glRenderMode( GL_SELECT );
		glInitNames();
	}

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glFrontFace( GL_CCW );

	glShadeModel( GL_SMOOTH );

	if( mode == GL_SELECT )
		glDisable( GL_LIGHTING );
	else
	{
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );
		glLightModelf( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
		glLightModelf( GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE );

		GLfloat lightPos[] = { ( GLfloat )camera.eye.get_e1(), ( GLfloat )camera.eye.get_e2(), ( GLfloat )camera.eye.get_e3(), 0.f };
		GLfloat lightColor[] = { 1.f, 1.f, 1.f, 1.f };
		glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
		glLightfv( GL_LIGHT0, GL_AMBIENT, lightColor );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, lightColor );
	}

	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	GLint viewport[4];
	glGetIntegerv( GL_VIEWPORT, viewport );
	double aspectRatio = double( viewport[2] ) / double( viewport[3] );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	if( mode == GL_SELECT )
		gluPickMatrix( GLdouble( mousePos.x ), GLdouble( viewport[3] - mousePos.y - 1 ), 2.0, 2.0, viewport );
	if( projection == PERSPECTIVE )
		gluPerspective( 60.0, aspectRatio, 0.1, 1000.0 );
	else if( projection == ORTHOGRAPHIC )
	{
		if( aspectRatio > 1.0 )
			glOrtho( -10.0 * aspectRatio, 10.0 * aspectRatio, -10.0, 10.0, 0.1, 1000.0 );
		else
			glOrtho( -10.0, 10.0, -10.0 / aspectRatio, 10.0 / aspectRatio, 0.1, 1000.0 );
	}

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	gluLookAt( camera.eye.get_e1(), camera.eye.get_e2(), camera.eye.get_e3(),
				0.0, 0.0, 0.0,
				camera.yAxis.get_e1(), camera.yAxis.get_e2(), camera.yAxis.get_e3() );
}

//==================================================================================================
void Canvas::PostRender( GLenum mode, bool selectFace )
{
	glFlush();

	if( mode == GL_RENDER )
		SwapBuffers();
	else if( mode == GL_SELECT )
	{
		int hitCount = glRenderMode( GL_RENDER );

		RubiksCube* rubiksCube = wxGetApp().rubiksCube;
		if( rubiksCube )
			grippingCube = rubiksCube->Select( hitBuffer, hitBufferSize, hitCount, ( selectFace ? 0 : &grip ), ( selectFace ? &selectedFaceId : 0 ) );

		delete[] hitBuffer;
		hitBuffer = 0;
		hitBufferSize = 0;

		wxFrame* frame = wxDynamicCast( GetParent(), wxFrame );
		if( frame )
		{
			wxString statusString;
			if( !selectFace )
			{
				if( !grippingCube )
					statusString = "No grip on cube.";
				else
				{
					wxString axisString[2];
					RubiksCube::TranslateAxis( axisString[0], grip.plane[0].axis );
					RubiksCube::TranslateAxis( axisString[1], grip.plane[1].axis );
					statusString = wxString::Format( "Grip plane 0: { %s, %d }; Grip plane 1: { %s, %d }.",
														axisString[0], grip.plane[0].index,
														axisString[1], grip.plane[1].index );
				}
			}
			else
			{
				if( selectedFaceId < 0 )
					statusString = "No face selected.";
				else
					statusString = wxString::Format( "Selected face with ID: %d", selectedFaceId );
			}

			wxStatusBar* statusBar = frame->GetStatusBar();
			statusBar->SetStatusText( statusString );
		}
	}
}

//==================================================================================================
void Canvas::OnPaint( wxPaintEvent& event )
{
	PreRender( GL_RENDER );

	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( rubiksCube )
		rubiksCube->Render( GL_RENDER, rotation, size, &selectedFaceId, comparativeRubiksCube, showInvariantFaces );

	if( showPerspectiveLabels )
		RenderPerspectiveLabels();

	PostRender( GL_RENDER, false );
}

//==================================================================================================
void Canvas::RenderPerspectiveLabels( void )
{
	glDisable( GL_TEXTURE_2D );

	bool lighting = glIsEnabled( GL_LIGHTING ) ? true : false;
	glDisable( GL_LIGHTING );

	RubiksCube::Perspective perspective;
	DeterminePerspective( perspective );
	
	double length = 0.7 * ( size.cubeWidthHeightAndDepth + size.subCubeWidthHeightAndDepth );

	glDisable( GL_DEPTH_TEST );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glColor3f( 0.5f, 0.5f, 0.5f );
//	RenderAxes( length, 1.f, 0.f );
	
	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	RenderAxisLabel( xAxis * length, AxisLabel( xAxis, perspective ) );
	RenderAxisLabel( yAxis * length, AxisLabel( yAxis, perspective ) );
	RenderAxisLabel( zAxis * length, AxisLabel( zAxis, perspective ) );

	RenderAxisLabel( -xAxis * length, AxisLabel( -xAxis, perspective ) );
	RenderAxisLabel( -yAxis * length, AxisLabel( -yAxis, perspective ) );
	RenderAxisLabel( -zAxis * length, AxisLabel( -zAxis, perspective ) );

	glEnable( GL_DEPTH_TEST );
	glDisable( GL_BLEND );

	if( lighting )
		glEnable( GL_LIGHTING );
}

//==================================================================================================
/*static*/ int Canvas::AxisLabel( const c3ga::vectorE3GA& axis, const RubiksCube::Perspective& perspective )
{
	double epsilon = 1e-7;
	if( c3ga::norm( axis - perspective.rAxis ) < epsilon )
		return 'R';
	if( c3ga::norm( axis + perspective.rAxis ) < epsilon )
		return 'L';
	if( c3ga::norm( axis - perspective.uAxis ) < epsilon )
		return 'U';
	if( c3ga::norm( axis + perspective.uAxis ) < epsilon )
		return 'D';
	if( c3ga::norm( axis - perspective.fAxis ) < epsilon )
		return 'F';
	if( c3ga::norm( axis + perspective.fAxis ) < epsilon )
		return 'B';
	return '?';
}

//==================================================================================================
void Canvas::RenderAxes( float length, float thickness, float radius )
{
	glLineWidth( thickness );

	float vertex[6][3] =
	{
		{ length, 0.f, 0.f },
		{ -length, 0.f, 0.f },
		{ 0.f, length, 0.f },
		{ 0.f, -length, 0.f },
		{ 0.f, 0.f, length },
		{ 0.f, 0.f, -length },
	};

	glBegin( GL_LINES );

	for( int i = 0; i < 6; i++ )
	{
		glVertex3f( 0.f, 0.f, 0.f );
		glVertex3fv( vertex[i] );
	}

	glEnd();

	if( radius > 0.f )
	{
		int triangleCount = 32;
		for( int i = 0; i < 6; i++ )
		{
			glBegin( GL_TRIANGLE_FAN );

			glVertex3fv( vertex[i] );
			for( int j = 0; j < triangleCount; j++ )
			{
				float angle = 2.f * M_PI * float(j) / float( triangleCount - 1 );
				float x = radius * cos( angle );
				float y = radius * sin( angle );

				c3ga::vectorE3GA haloVertex( c3ga::vectorE3GA::coord_e1_e2_e3, vertex[i][0], vertex[i][1], vertex[i][2] );
				haloVertex += c3ga::gp( camera.xAxis, x ) + c3ga::gp( camera.yAxis, y );
				glVertex3d( haloVertex.get_e1(), haloVertex.get_e2(), haloVertex.get_e3() );
			}

			glEnd();
		}
	}
}

//==================================================================================================
void Canvas::RenderAxisLabel( const c3ga::vectorE3GA& axis, int label )
{
	float delta = 0.0f;		// I'm not sure how to calculate this.
	glRasterPos3f( axis.get_e1() - delta, axis.get_e2() - delta, axis.get_e3() - delta );

	PerspectiveLabelMap::iterator iter = perspectiveLabelMap.find( label );
	if( iter == perspectiveLabelMap.end() )
	{
		wxString perspectiveLabelFile = wxString::Format( "Textures/label_%c.png", ( char )label );

		wxImage* image = new wxImage();
		if( !image->LoadFile( perspectiveLabelFile, wxBITMAP_TYPE_PNG ) )
			delete image;
		else
		{
			RGBA_Buffer* buffer = new RGBA_Buffer( *image );
			perspectiveLabelMap[ label ] = buffer;
			iter = perspectiveLabelMap.find( label );
		}
	}

	if( iter == perspectiveLabelMap.end() )
		return;

	RGBA_Buffer* buffer = iter->second;
	glDrawPixels( buffer->width, buffer->height, GL_RGBA, GL_UNSIGNED_BYTE, buffer->pixelData );
}

//==================================================================================================
void Canvas::DeletePerspectiveLabelMap( void )
{
	while( perspectiveLabelMap.size() > 0 )
	{
		PerspectiveLabelMap::iterator iter = perspectiveLabelMap.begin();
		RGBA_Buffer* buffer = iter->second;
		delete buffer;
		perspectiveLabelMap.erase( iter );
	}
}

//==================================================================================================
void Canvas::OnSize( wxSizeEvent& event )
{
	if( IsShown() )
	{
		//BindContext();
		wxSize size = event.GetSize();
		glViewport( 0, 0, size.GetWidth(), size.GetHeight() );

		Refresh();
	}
}

//==================================================================================================
void Canvas::OnMouseLeftDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();

	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( rubiksCube && event.ControlDown() )
	{
		PreRender( GL_SELECT );
		rubiksCube->Render( GL_SELECT, rotation, size );
		PostRender( GL_SELECT, true );
		Refresh();
	}
}

//==================================================================================================
void Canvas::OnMouseRightDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();

	// Perform selection so that we know what kind of grip, if any, we have on the cube.
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( rubiksCube )
	{
		PreRender( GL_SELECT );
		rubiksCube->Render( GL_SELECT, rotation, size );
		PostRender( GL_SELECT, false );

		if( grippingCube )
			CaptureMouse();
	}
}

//==================================================================================================
void Canvas::OnMouseRightUp( wxMouseEvent& event )
{
	if( grippingCube )
	{
		ReleaseMouse();

		ApplyRotation( rotation );

		grippingCube = false;
	}
}

//==================================================================================================
void Canvas::OnMouseMotion( wxMouseEvent& event )
{
	if( event.LeftIsDown() )
	{
		wxPoint mouseDelta = event.GetPosition() - mousePos;
		mousePos = event.GetPosition();

		if( mouseDelta.x == 0 && mouseDelta.y == 0 )
			return;

		double mouseSensativity = 0.2;
		c3ga::vectorE3GA strafeVector = c3ga::gp( c3ga::gp( camera.xAxis, -double( mouseDelta.x ) ) +
													c3ga::gp( camera.yAxis, double( mouseDelta.y ) ), mouseSensativity );

		double focalDistance = c3ga::norm( camera.eye );
		c3ga::vectorE3GA eye = camera.eye + strafeVector;
		eye = c3ga::gp( eye, focalDistance / c3ga::norm( eye ) );

		double angle = acos( c3ga::lc( camera.eye, eye ) / ( focalDistance * focalDistance ) );
		c3ga::bivectorE3GA blade = c3ga::op( camera.eye, eye );
		c3ga::rotorE3GA rotor = c3ga::exp( c3ga::unit( blade ) * -0.5 * angle );

		double distance = c3ga::norm( c3ga::applyUnitVersor( rotor, camera.eye ) - eye );
		wxASSERT( distance < 1e-7 );

		camera.xAxis = c3ga::applyUnitVersor( rotor, camera.xAxis );
		camera.yAxis = c3ga::applyUnitVersor( rotor, camera.yAxis );
		camera.eye = eye;

		// At this point, the movement of the camera is finished, but we'll want
		// to account for accumulated round off error here by renormalizing stuff.

		c3ga::vectorE3GA zAxis = c3ga::gp( eye, -1.0 / focalDistance );
		camera.xAxis = c3ga::unit( c3ga::rc( c3ga::op( camera.xAxis, zAxis ), zAxis ) );
		camera.yAxis = c3ga::unit( c3ga::rc( c3ga::op( camera.yAxis, zAxis ), zAxis ) );

		Refresh();
	}
	else if( event.RightIsDown() && grippingCube )
	{
		wxPoint mouseDelta = event.GetPosition() - mousePos;

		c3ga::bivectorE3GA blade = camera.xAxis ^ camera.yAxis;
		c3ga::vectorE3GA zAxis = c3ga::lc( blade, c3ga::trivectorE3GA( c3ga::trivectorE3GA::coord_e1e2e3, -1.0 ) );

		c3ga::vectorE3GA axis0 = RubiksCube::TranslateAxis( grip.plane[0].axis );
		c3ga::vectorE3GA axis1 = RubiksCube::TranslateAxis( grip.plane[1].axis );

		c3ga::vectorE3GA projectedAxis0 = c3ga::unit( c3ga::rc( blade, c3ga::lc( axis0, blade ) ) );
		c3ga::vectorE3GA projectedAxis1 = c3ga::unit( c3ga::rc( blade, c3ga::lc( axis1, blade ) ) );

		double mouseSensativity = 0.015;
		c3ga::vectorE3GA motionVector = c3ga::gp( c3ga::gp( camera.xAxis, double( mouseDelta.x ) ) +
													c3ga::gp( camera.yAxis, -double( mouseDelta.y ) ), mouseSensativity );

		c3ga::vectorE3GA unitMotionVector = c3ga::unit( motionVector );

		double dot0 = c3ga::lc( unitMotionVector, projectedAxis0 );
		double dot1 = c3ga::lc( unitMotionVector, projectedAxis1 );

		double rotationAngle = c3ga::norm( motionVector );

		double fn0 = fabs( dot0 - 1.0 );
		double fp0 = fabs( dot0 + 1.0 );
		double fn1 = fabs( dot1 - 1.0 );
		double fp1 = fabs( dot1 + 1.0 );

		double test0 = fn0 < fp1 ? fn0 : fp0;
		double test1 = fn1 < fp1 ? fn1 : fp1;

		c3ga::trivectorE3GA rotationDirection;
		if( test0 < test1 )
		{
			rotation.plane = grip.plane[1];
			rotationDirection = projectedAxis1 ^ zAxis ^ motionVector;
		}
		else
		{
			rotation.plane = grip.plane[0];
			rotationDirection = projectedAxis0 ^ zAxis ^ motionVector;
		}

		rotationDirection = c3ga::unit( rotationDirection );
		rotation.angle = rotationAngle * rotationDirection.get_e1_e2_e3();

		Refresh();
	}
}

//==================================================================================================
void Canvas::OnMouseCaptureLost( wxMouseCaptureLostEvent& event )
{
	grippingCube = false;
}

//==================================================================================================
void Canvas::DeterminePerspective( RubiksCube::Perspective& perspective ) const
{
	perspective.rAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	perspective.uAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	perspective.fAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	AxisList axisList;
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 0.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, -1.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, -1.0 ) );

	c3ga::trivectorE3GA I( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 );
	c3ga::vectorE3GA zAxis;
	zAxis = c3ga::gp( camera.xAxis ^ camera.yAxis, -I );

	// Find the forward axis.  This is the axis pointing most in the direction of the camera.
	double maximumDot = -2.0;
	for( AxisList::iterator iter = axisList.begin(); iter != axisList.end(); iter++ )
	{
		c3ga::vectorE3GA axis = *iter;
		double dot = c3ga::lc( axis, zAxis );
		if( dot > maximumDot )
		{
			maximumDot = dot;
			perspective.fAxis = axis;
		}
	}

	// Cull from our axis-list all axes that are not orthogonal to the forward axis.
	double epsilon = 1e-7;
	AxisList::iterator nextIter;
	for( AxisList::iterator iter = axisList.begin(); iter != axisList.end(); iter = nextIter )
	{
		nextIter = iter;
		nextIter++;
		c3ga::vectorE3GA axis = *iter;
		double dot = c3ga::lc( axis, perspective.fAxis );
		if( !( fabs( dot ) < epsilon ) )
			axisList.erase( iter );
	}

	// Determine the right-axis.
	c3ga::vectorE3GA vec0 = camera.xAxis + camera.yAxis;
	c3ga::vectorE3GA vec1 = camera.xAxis - camera.yAxis;
	c3ga::bivectorE3GA blade0, blade1;
	blade0 = c3ga::gp( c3ga::unit( vec0 ), I );
	blade1 = c3ga::gp( c3ga::unit( vec1 ), I );
	AxisList rightOrDownAxisList, rightAxisList;
	SortAxesByBlade( axisList, blade0, 0, &rightOrDownAxisList, &rightOrDownAxisList );
	SortAxesByBlade( rightOrDownAxisList, blade1, 0, &rightAxisList, &rightAxisList );
	//wxASSERT( rightAxisList.size() == 1 );
	if( rightAxisList.size() > 0 )
		perspective.rAxis = rightAxisList.front();
	else if( rightOrDownAxisList.size() > 0 )
		perspective.rAxis = rightOrDownAxisList.front();
	else
		perspective.rAxis = axisList.front();

	// Lastly, form a right-handed system.
	perspective.uAxis = c3ga::gp( perspective.fAxis ^ perspective.rAxis, -I );
}

//==================================================================================================
/*static*/ void Canvas::SortAxesByBlade( const AxisList& axisList, const c3ga::bivectorE3GA& blade,
													AxisList* backAxisList,
													AxisList* frontAxisList,
													AxisList* neitherAxisList )
{
	for( AxisList::const_iterator iter = axisList.begin(); iter != axisList.end(); iter++ )
	{
		c3ga::vectorE3GA axis = *iter;
		c3ga::trivectorE3GA trivector = blade ^ axis;

		if( trivector.get_e1_e2_e3() > 0.0 )
		{
			if( frontAxisList )
				frontAxisList->push_back( axis );
		}
		else if( trivector.get_e1_e2_e3() < 0.0 )
		{
			if( backAxisList )
				backAxisList->push_back( axis );
		}
		else
		{
			if( neitherAxisList )
				neitherAxisList->push_back( axis );
		}
	}
}

//==================================================================================================
Canvas::RGBA_Buffer::RGBA_Buffer( const wxImage& image )
{
	wxASSERT( image.HasAlpha() );

	width = image.GetWidth();
	height = image.GetHeight();

	pixelData = new GLubyte[ width * height * 4 ];

	for( int i = 0; i < height; i++ )
	{
		for( int j = 0; j < width; j++ )
		{
			GLubyte* pixel = &pixelData[ ( ( height - 1 - i ) * width + j ) * 4 ];
			const GLubyte* imageData = image.GetData();
			pixel[0] = imageData[ ( i * width + j ) * 3 + 0 ];
			pixel[1] = imageData[ ( i * width + j ) * 3 + 1 ];
			pixel[2] = imageData[ ( i * width + j ) * 3 + 2 ];
			pixel[3] = image.GetAlpha()[ i * width + j ];
		}
	}
}

Canvas::RGBA_Buffer::~RGBA_Buffer( void )
{
	delete[] pixelData;
}

// Canvas.cpp
