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
}

//==================================================================================================
/*virtual*/ Canvas::~Canvas( void )
{
	delete context;
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

	if( rotation.angle == 0.0 )
		return false;

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
// Note that the applied move is not put on the move stack.
bool Canvas::ApplyRotation( const RubiksCube::Rotation& rotation )
{
	RubiksCube* rubiksCube = wxGetApp().rubiksCube;
	if( !rubiksCube )
		return false;

	return rubiksCube->Apply( rotation, &this->rotation );
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
	gluPerspective( 60.0, aspectRatio, 0.1, 1000.0 );

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

	PostRender( GL_RENDER );
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

		RubiksCube* rubiksCube = wxGetApp().rubiksCube;
		if( rubiksCube )
		{
			RubiksCube::Rotation reverseRotation;
			if( rubiksCube->Apply( rotation, &rotation, &reverseRotation ) )
				wxGetApp().rotationStack.push_back( reverseRotation );
		}

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

// Canvas.cpp