// Canvas.cpp

#include "Header.h"

int Canvas::attributeList[] = { WX_GL_RGBA, WX_GL_DOUBLEBUFFER, 0 };

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

	camera.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 12.0 );
	camera.xAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
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
}

//==================================================================================================
/*virtual*/ Canvas::~Canvas( void )
{
	delete context;
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
bool Canvas::ApplyRotation( const RubiksCube::Rotation& rotation, bool pushReverseRotation /*= true*/ )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return false;

	RubiksCube::Rotation reverseRotation;
	if( !rubiksCube->Apply( rotation, &this->rotation, &reverseRotation ) )
		return false;

	if( pushReverseRotation )
		wxGetApp().PushRotation( reverseRotation );

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

		GLfloat lightPos[] = { camera.eye.get_e1(), camera.eye.get_e2(), camera.eye.get_e3(), 0.f };
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
void Canvas::PostRender( GLenum mode )
{
	glFlush();

	if( mode == GL_RENDER )
		SwapBuffers();
	else if( mode == GL_SELECT )
	{
		int hitCount = glRenderMode( GL_RENDER );

		RubiksCube* rubiksCube = wxGetApp().rubiksCube;
		if( rubiksCube )
			grippingCube = rubiksCube->Select( hitBuffer, hitBufferSize, hitCount, grip );

		delete[] hitBuffer;
		hitBuffer = 0;
		hitBufferSize = 0;

		wxFrame* frame = wxDynamicCast( GetParent(), wxFrame );
		if( frame )
		{
			wxString gripString;
			if( !grippingCube )
				gripString = "No grip on cube.";
			else
			{
				wxString axisString[2];
				RubiksCube::TranslateAxis( axisString[0], grip.plane[0].axis );
				RubiksCube::TranslateAxis( axisString[1], grip.plane[1].axis );
				gripString = wxString::Format( "Grip plane 0: { %s, %d }; Grip plane 1: { %s, %d }.",
													axisString[0], grip.plane[0].index,
													axisString[1], grip.plane[1].index );
			}

			wxStatusBar* statusBar = frame->GetStatusBar();
			statusBar->SetStatusText( gripString );
		}
	}
}

//==================================================================================================
void Canvas::OnPaint( wxPaintEvent& event )
{
	PreRender( GL_RENDER );

	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( rubiksCube )
		rubiksCube->Render( GL_RENDER, rotation, size );

	if( showPerspectiveLabels )
		RenderPerspectiveLabels();

	PostRender( GL_RENDER );
}

//==================================================================================================
void Canvas::RenderPerspectiveLabels( void )
{
	bool lighting = glIsEnabled( GL_LIGHTING ) ? true : false;
	glDisable( GL_LIGHTING );

	RubiksCube::Perspective perspective;
	DeterminePerspective( perspective );
	
	double length = 0.7 * ( size.cubeWidthHeightAndDepth + size.subCubeWidthHeightAndDepth );

	glDisable( GL_DEPTH_TEST );

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
	glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, label );
}

//==================================================================================================
void Canvas::OnSize( wxSizeEvent& event )
{
	BindContext();
	wxSize size = event.GetSize();
	glViewport( 0, 0, size.GetWidth(), size.GetHeight() );

	Refresh();
}

//==================================================================================================
void Canvas::OnMouseLeftDown( wxMouseEvent& event )
{
	mousePos = event.GetPosition();
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
		PostRender( GL_SELECT );

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

	typedef std::list< c3ga::vectorE3GA > AxisList;
	AxisList axisList;

	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 0.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, -1.0, 0.0 ) );
	axisList.push_back( c3ga::vectorE3GA( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, -1.0 ) );

	c3ga::vectorE3GA zAxis;
	c3ga::trivectorE3GA I( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 );
	c3ga::bivectorE3GA viewBlade = camera.xAxis ^ camera.yAxis;
	zAxis = c3ga::gp( viewBlade, -I );
	c3ga::bivectorE3GA verticalViewBlade = camera.yAxis ^ zAxis;
	c3ga::bivectorE3GA horizontalViewBlade = zAxis ^ camera.xAxis;

	AxisList visibleAxisList, almostVisibleAxisList;
	SortAxesByBlade( axisList, viewBlade, 0, &visibleAxisList, &almostVisibleAxisList );

	if( visibleAxisList.size() == 3 )
	{
		AxisList rightAxisList, forwardAxisList, neitherAxisList;
		SortAxesByBlade( visibleAxisList, verticalViewBlade, &forwardAxisList, &rightAxisList, &neitherAxisList );
		wxASSERT( rightAxisList.size() > 0 );
		wxASSERT( forwardAxisList.size() > 0 );
		wxASSERT( neitherAxisList.size() != 2 );

		if( rightAxisList.size() == 1 && forwardAxisList.size() == 1 && neitherAxisList.size() == 1 )
		{
			perspective.rAxis = rightAxisList.front();
			perspective.fAxis = forwardAxisList.front();

			perspective.uAxis = neitherAxisList.front();
			c3ga::trivectorE3GA trivector = perspective.rAxis ^ perspective.fAxis ^ perspective.uAxis;
			if( trivector.get_e1_e2_e3() > 0.0 )
				perspective.uAxis = -perspective.uAxis;
		}
		else if( rightAxisList.size() == 1 && forwardAxisList.size() == 2 )
		{
			perspective.rAxis = rightAxisList.front();

			AxisList aboveAxisList, belowAxisList;
			SortAxesByBlade( forwardAxisList, horizontalViewBlade, &belowAxisList, &aboveAxisList, 0 );
			wxASSERT( aboveAxisList.size() == 1 );
			wxASSERT( belowAxisList.size() == 1 );

			perspective.uAxis = aboveAxisList.front();
			perspective.fAxis = belowAxisList.front();
		}
		else if( rightAxisList.size() == 2 && forwardAxisList.size() == 1 )
		{
			perspective.fAxis = forwardAxisList.front();

			AxisList aboveAxisList, belowAxisList;
			SortAxesByBlade( rightAxisList, horizontalViewBlade, &belowAxisList, &aboveAxisList, 0 );
			wxASSERT( aboveAxisList.size() == 1 );
			wxASSERT( belowAxisList.size() == 1 );

			perspective.uAxis = aboveAxisList.front();
			perspective.rAxis = belowAxisList.front();
		}
	}
	else if( visibleAxisList.size() == 2 )
	{
		AxisList rightAxisList, forwardAxisList, neitherAxisList;
		SortAxesByBlade( visibleAxisList, verticalViewBlade, &forwardAxisList, &rightAxisList, &neitherAxisList );
		wxASSERT( neitherAxisList.size() == 0 || neitherAxisList.size() == 2 );
		if( neitherAxisList.size() == 0 )
		{
			perspective.rAxis = rightAxisList.front();
			perspective.fAxis = forwardAxisList.front();
			perspective.uAxis = c3ga::gp( perspective.fAxis ^ perspective.rAxis, -I );
		}
		else
		{
			neitherAxisList.clear();
			AxisList aboveAxisList, belowAxisList;
			SortAxesByBlade( visibleAxisList, horizontalViewBlade, &belowAxisList, &aboveAxisList, &neitherAxisList );
			wxASSERT( neitherAxisList.size() == 0 );
			wxASSERT( aboveAxisList.size() == 1 );
			wxASSERT( belowAxisList.size() == 1 );
			perspective.uAxis = aboveAxisList.front();
			perspective.fAxis = belowAxisList.front();
			perspective.rAxis = c3ga::gp( perspective.uAxis ^ perspective.fAxis, -I );
		}
	}
	else if( visibleAxisList.size() == 1 )
	{
		wxASSERT( almostVisibleAxisList.size() == 4 );
		//...
		return;
	}

	// We should always be returning a right-handed system.
	c3ga::trivectorE3GA trivector = perspective.rAxis ^ perspective.uAxis ^ perspective.fAxis;
	wxASSERT( trivector.get_e1_e2_e3() > 0.0 );
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

// Canvas.cpp