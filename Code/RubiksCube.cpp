// RubiksCube.cpp

#include "Header.h"

//==================================================================================================
RubiksCube::RubiksCube( int subCubeMatrixSize /*= 3*/, int colorCount /*= 6*/ )
{
	this->subCubeMatrixSize = subCubeMatrixSize;
	subCubeMatrix = new SubCube**[ subCubeMatrixSize ];
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		subCubeMatrix[x] = new SubCube*[ subCubeMatrixSize ];
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			subCubeMatrix[x][y] = new SubCube[ subCubeMatrixSize ];
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				SubCube* subCube = &subCubeMatrix[x][y][z];

				subCube->faceColor[ NEG_X ] = ( x == 0 ) ? ( colorCount > 1 ? YELLOW : WHITE ) : GREY;
				subCube->faceColor[ POS_X ] = ( x == subCubeMatrixSize - 1 ) ? ( colorCount > 0 ? RED : WHITE ) : GREY;
				subCube->faceColor[ NEG_Y ] = ( y == 0 ) ? ( colorCount > 3 ? MEGENTA : WHITE ) : GREY;
				subCube->faceColor[ POS_Y ] = ( y == subCubeMatrixSize - 1 ) ? ( colorCount > 2 ? GREEN : WHITE ) : GREY;
				subCube->faceColor[ NEG_Z ] = ( z == 0 ) ? ( colorCount > 5 ? CYAN : WHITE ) : GREY;
				subCube->faceColor[ POS_Z ] = ( z == subCubeMatrixSize - 1 ) ? ( colorCount > 4 ? BLUE : WHITE ) : GREY;
			}
		}
	}
}

//==================================================================================================
RubiksCube::~RubiksCube( void )
{
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
			delete[] subCubeMatrix[x][y];
		delete[] subCubeMatrix[x];
	}
	delete[] subCubeMatrix;
}

//==================================================================================================
int RubiksCube::SubCubeMatrixSize( void ) const
{
	return subCubeMatrixSize;
}

//==================================================================================================
void RubiksCube::Render( GLenum mode, const Rotation& rotation, const Size& size ) const
{
	double t;
	c3ga::vectorE3GA subCubeCenter;

	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		t = double( x ) / double( subCubeMatrixSize - 1 );
		subCubeCenter.set_e1( size.cubeWidthHeightAndDepth * ( t - 0.5 ) );

		if( mode == GL_SELECT )
			glPushName( x );

		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			t = double( y ) / double( subCubeMatrixSize - 1 );
			subCubeCenter.set_e2( size.cubeWidthHeightAndDepth * ( t - 0.5 ) );

			if( mode == GL_SELECT )
				glPushName( y );

			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				t = double( z ) / double( subCubeMatrixSize - 1 );
				subCubeCenter.set_e3( size.cubeWidthHeightAndDepth * ( t - 0.5 ) );

				if( mode == GL_SELECT )
					glPushName( z );

				c3ga::rotorE3GA rotor( c3ga::rotorE3GA::coord_scalar_e1e2_e2e3_e3e1, 1.0, 0.0, 0.0, 0.0 );

				if( ( rotation.plane.axis == X_AXIS && x == rotation.plane.index ) ||
					( rotation.plane.axis == Y_AXIS && y == rotation.plane.index ) ||
					( rotation.plane.axis == Z_AXIS && z == rotation.plane.index ) )
				{
					c3ga::vectorE3GA rotationAxis = TranslateAxis( rotation.plane.axis );
					double rotationAngle = rotation.angle;
					c3ga::bivectorE3GA rotationBlade;
					rotationBlade.set( c3ga::lc( rotationAxis, c3ga::trivectorE3GA( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 ) ) );
					rotor = c3ga::exp( rotationBlade * -0.5 * rotationAngle );
				}

				c3ga::mv motor = 1.0 - 0.5 * subCubeCenter * c3ga::ni;

				double scale = double( size.subCubeWidthHeightAndDepth ) * 0.5;
				c3ga::mv dilator = ( c3ga::no - 0.5 * scale * c3ga::ni ) * ( c3ga::no - 0.5 * c3ga::ni );

				// Notice that the rotor is applied last, because we want the sub-cube
				// to rotate about an axis that goes through the center of the entire cube.
				c3ga::evenVersor vertexVersor;
				vertexVersor.set( rotor * motor * dilator );
				c3ga::evenVersor normalVersor;
				normalVersor.set( rotor );

				SubCube* subCube = &subCubeMatrix[x][y][z];
				RenderSubCube( mode, subCube, vertexVersor, normalVersor );

				if( mode == GL_SELECT )
					glPopName();
			}

			if( mode == GL_SELECT )
				glPopName();
		}

		if( mode == GL_SELECT )
			glPopName();
	}
}

//==================================================================================================
double RubiksCube::subCubeVertex[8][3] =
{
	{ -1.0, -1.0, -1.0 }, // 0
	{  1.0, -1.0, -1.0 }, // 1
	{ -1.0,  1.0, -1.0 }, // 2
	{  1.0,  1.0, -1.0 }, // 3
	{ -1.0, -1.0,  1.0 }, // 4
	{  1.0, -1.0,  1.0 }, // 5
	{ -1.0,  1.0,  1.0 }, // 6
	{  1.0,  1.0,  1.0 }, // 7
};

//==================================================================================================
// Here we must insure that the winding order is such that back-face culling will work.
int RubiksCube::subCubeFace[ RubiksCube::CUBE_FACE_COUNT ][4] =
{
	{ 0, 4, 6, 2 }, // NEG_X
	{ 1, 3, 7, 5 }, // POS_X
	{ 0, 1, 5, 4 }, // NEG_Y
	{ 2, 6, 7, 3 }, // POS_Y
	{ 0, 2, 3, 1 }, // NEG_Z
	{ 4, 5, 7, 6 }, // POS_Z
};

//==================================================================================================
void RubiksCube::RenderSubCube( GLenum mode, const SubCube* subCube, const c3ga::evenVersor& vertexVersor, const c3ga::evenVersor& normalVersor ) const
{
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
	{
		if( mode == GL_SELECT )
			glPushName( face );

		c3ga::vectorE3GA faceColor = TranslateColor( subCube->faceColor[ face ] );
		if( GL_FALSE == glIsEnabled( GL_LIGHTING ) )
			glColor3d( faceColor.get_e1(), faceColor.get_e2(), faceColor.get_e3() );
		else
		{
			c3ga::vectorE3GA faceNormal = TranslateNormal( Face( face ) );
			faceNormal = c3ga::applyUnitVersor( normalVersor, faceNormal );
			glNormal3f( faceNormal.get_e1(), faceNormal.get_e2(), faceNormal.get_e3() );

			GLfloat ambientDiffuse[] = { faceColor.get_e1(), faceColor.get_e2(), faceColor.get_e3(), 1.f };
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientDiffuse );
		}

		glBegin( GL_QUADS );

		for( int index = 0; index < 4; index++ )
		{
			double* vertex = subCubeVertex[ subCubeFace[ face ][ index ] ];
			c3ga::vectorE3GA point( c3ga::vectorE3GA::coord_e1_e2_e3, vertex[0], vertex[1], vertex[2] );
			c3ga::dualSphere dualPoint;
			dualPoint.set( c3ga::no + point + 0.5 * c3ga::norm2( point ) * c3ga::ni );
			dualPoint = c3ga::applyUnitVersor( vertexVersor, dualPoint );		// This should always be homogenized.
			glVertex3d( dualPoint.get_e1(), dualPoint.get_e2(), dualPoint.get_e3() );
		}

		glEnd();

		if( mode == GL_SELECT )
			glPopName();
	}
}

//==================================================================================================
/*static*/ c3ga::vectorE3GA RubiksCube::TranslateColor( Color color )
{
	c3ga::vectorE3GA colorVector( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 );

	switch( color )
	{
		case GREY:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.5, 0.5, 0.5 ); break;
		case RED:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 ); break;
		case GREEN:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 ); break;
		case BLUE:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 ); break;
		case YELLOW:	colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 0.0 ); break;
		case MEGENTA:	colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 1.0 ); break;
		case CYAN:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 1.0 ); break;
		case WHITE:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 ); break;
	}

	return colorVector;
}

//==================================================================================================
/*static*/ c3ga::vectorE3GA RubiksCube::TranslateAxis( Axis axis )
{
	c3ga::vectorE3GA axisVector( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	switch( axis )
	{
		case X_AXIS:	axisVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 ); break;
		case Y_AXIS:	axisVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 ); break;
		case Z_AXIS:	axisVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 ); break;
	}

	return axisVector;
}

//==================================================================================================
/*static*/ void RubiksCube::TranslateAxis( wxString& axisString, Axis axis )
{
	axisString = "???";

	switch( axis )
	{
		case X_AXIS: axisString = "X-axis"; break;
		case Y_AXIS: axisString = "Y-axis"; break;
		case Z_AXIS: axisString = "Z-axis"; break;
	}
}

//==================================================================================================
/*static*/ c3ga::vectorE3GA RubiksCube::TranslateNormal( Face face )
{
	c3ga::vectorE3GA faceNormal( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	switch( face )
	{
		case NEG_X: faceNormal.set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0,  0.0,  0.0 ); break;
		case POS_X: faceNormal.set( c3ga::vectorE3GA::coord_e1_e2_e3,  1.0,  0.0,  0.0 ); break;
		case NEG_Y: faceNormal.set( c3ga::vectorE3GA::coord_e1_e2_e3,  0.0, -1.0,  0.0 ); break;
		case POS_Y: faceNormal.set( c3ga::vectorE3GA::coord_e1_e2_e3,  0.0,  1.0,  0.0 ); break;
		case NEG_Z: faceNormal.set( c3ga::vectorE3GA::coord_e1_e2_e3,  0.0,  0.0, -1.0 ); break;
		case POS_Z: faceNormal.set( c3ga::vectorE3GA::coord_e1_e2_e3,  0.0,  0.0,  1.0 ); break;
	}

	return faceNormal;
}

//==================================================================================================
/*static*/ bool RubiksCube::TranslateGrip( Grip& grip, int x, int y, int z, Face face )
{
	// If I'm touching the given face of the given sub-cube, what are
	// the two rotation planes that I have a grip on?
	switch( face )
	{
		case NEG_X:
		case POS_X:
		{
			grip.plane[0].axis = Y_AXIS;
			grip.plane[0].index = y;
			grip.plane[1].axis = Z_AXIS;
			grip.plane[1].index = z;
			return true;
		}
		case NEG_Y:
		case POS_Y:
		{
			grip.plane[0].axis = X_AXIS;
			grip.plane[0].index = x;
			grip.plane[1].axis = Z_AXIS;
			grip.plane[1].index = z;
			return true;
		}
		case NEG_Z:
		case POS_Z:
		{
			grip.plane[0].axis = X_AXIS;
			grip.plane[0].index = x;
			grip.plane[1].axis = Y_AXIS;
			grip.plane[1].index = y;
			return true;
		}
	}

	return false;
}

//==================================================================================================
bool RubiksCube::Apply( const Rotation& rotation, Rotation* invariantRotation /*= 0*/, Rotation* reverseRotation /*= 0*/ )
{
	double rotationAngle = fmod( rotation.angle, 2.0 * M_PI );

	if( reverseRotation )
		reverseRotation->plane = rotation.plane;

	int rotationCount = 0;
	RotationDirection rotationDirection;
	double angle;
	if( rotation.angle > 0.0 )
	{
		rotationDirection = ROTATE_CCW;
		for( angle = 0.0; fabs( angle - rotationAngle ) > M_PI / 4.0; angle += M_PI / 2.0 )
			rotationCount++;
		if( reverseRotation )
			reverseRotation->angle = double( rotationCount ) * -M_PI / 2.0;
	}
	else if( rotation.angle < 0.0 )
	{
		rotationDirection = ROTATE_CW;
		for( angle = 0.0; fabs( angle - rotationAngle ) > M_PI / 4.0; angle -= M_PI / 2.0 )
			rotationCount++;
		if( reverseRotation )
			reverseRotation->angle = double( rotationCount ) * M_PI / 2.0;
	}

	if( rotationCount == 0 )
		return false;

	RotatePlane( rotation.plane, rotationDirection, rotationCount );

	// Notice that the invariant rotation is used as both input and output.
	if( invariantRotation )
	{
		invariantRotation->plane = rotation.plane;
		invariantRotation->angle -= angle;
	}

	return true;
}

//==================================================================================================
bool RubiksCube::ApplySequence( const RotationSequence& rotationSequence )
{
	RotationSequence rotationList = rotationSequence;
	while( rotationList.size() > 0 )
	{
		RotationSequence::iterator iter = rotationList.begin();
		Rotation rotation = *iter;
		rotationList.erase( iter );
		if( !Apply( rotation ) )
			return false;
	}
	return true;
}

//==================================================================================================
void RubiksCube::RotatePlane( Plane plane, RotationDirection rotationDirection, int rotationCount )
{
	// To achieve the desired rotation, we always rotate some number of times counter-clock-wise.
	int counterClockwiseRotationCount;
	if( rotationDirection == ROTATE_CCW )
		counterClockwiseRotationCount = rotationCount % 4;
	else if( rotationDirection == ROTATE_CW )
		counterClockwiseRotationCount = ( 4 - rotationCount % 4 ) % 4;

	while( counterClockwiseRotationCount > 0 )
	{
		RotatePlaneCCW( plane );
		counterClockwiseRotationCount--;
	}
}

//==================================================================================================
// Use what we know about symmetry groups to perform the rotation.
void RubiksCube::RotatePlaneCCW( Plane plane )
{
	// Flip the plane vertically.
	for( int i = 0; i < subCubeMatrixSize; i++ )
		for( int j = 0; j < ( subCubeMatrixSize - ( subCubeMatrixSize % 2 ) ) / 2; j++ )
			SwapSubCubes( SubCubeIndexPlane( plane, i, j ), SubCubeIndexPlane( plane, i, subCubeMatrixSize - j - 1 ) );

	// Flip each sub-cube vertically.
	for( int i = 0; i < subCubeMatrixSize; i++ )
	{
		for( int j = 0; j < subCubeMatrixSize; j++ )
		{
			SubCube* subCube = SubCubeIndexPlane( plane, i, j );
			switch( plane.axis )
			{
				case X_AXIS: SwapColors( subCube, NEG_Z, POS_Z ); break;
				case Y_AXIS: SwapColors( subCube, NEG_X, POS_X ); break;
				case Z_AXIS: SwapColors( subCube, NEG_Y, POS_Y ); break;
			}
		}
	}
	
	// Flip the plane diagonally.
	for( int i = 0; i < subCubeMatrixSize; i++ )
		for( int j = 0; j < i; j++ )
			SwapSubCubes( SubCubeIndexPlane( plane, i, j ), SubCubeIndexPlane( plane, j, i ) );

	// Flip each sub-cube diagonally.
	for( int i = 0; i < subCubeMatrixSize; i++ )
	{
		for( int j = 0; j < subCubeMatrixSize; j++ )
		{
			SubCube* subCube = SubCubeIndexPlane( plane, i, j );
			switch( plane.axis )
			{
				case X_AXIS: SwapColors( subCube, NEG_Y, NEG_Z ); SwapColors( subCube, POS_Y, POS_Z ); break;
				case Y_AXIS: SwapColors( subCube, NEG_Z, NEG_X ); SwapColors( subCube, POS_Z, POS_X ); break;
				case Z_AXIS: SwapColors( subCube, NEG_X, NEG_Y ); SwapColors( subCube, POS_X, POS_Y ); break;
			}
		}
	}
}

//==================================================================================================
RubiksCube::SubCube* RubiksCube::SubCubeIndexPlane( Plane plane, int i, int j )
{
	switch( plane.axis )
	{
		case X_AXIS: return &subCubeMatrix[ plane.index ][ i ][ j ];
		case Y_AXIS: return &subCubeMatrix[ j ][ plane.index ][ i ];
		case Z_AXIS: return &subCubeMatrix[ i ][ j ][ plane.index ];
	}

	return 0;
}

//==================================================================================================
/*static*/ void RubiksCube::SwapColors( SubCube* subCube, int i, int j )
{
	Color tempColor = subCube->faceColor[i];
	subCube->faceColor[i] = subCube->faceColor[j];
	subCube->faceColor[j] = tempColor;
}

//==================================================================================================
/*static*/ void RubiksCube::SwapSubCubes( SubCube* subCube0, SubCube* subCube1 )
{
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
	{
		Color tempColor = subCube0->faceColor[ face ];
		subCube0->faceColor[ face ] = subCube1->faceColor[ face ];
		subCube1->faceColor[ face ] = tempColor;
	}
}

//==================================================================================================
bool RubiksCube::Select( unsigned int* hitBuffer, int hitBufferSize, int hitCount, Grip& grip ) const
{
	bool grippingCube = false;

	// Notice that if "hitCount" is -1, which indicates an
	// over-flow of the buffer, we don't process anything.
	unsigned int* hitRecord = hitBuffer;
	float smallestZ = 999.f;
	for( int hit = 0; hit < hitCount; hit++ )
	{
		unsigned int nameCount = hitRecord[0];
		wxASSERT( nameCount == 4 );
		if( nameCount == 4 )
		{
			int x = ( int )hitRecord[3];
			int y = ( int )hitRecord[4];
			int z = ( int )hitRecord[5];
			Face face = ( Face )hitRecord[6];

			const SubCube* subCube = &subCubeMatrix[x][y][z];
			if( subCube->faceColor[ face ] != GREY )
			{
				float minZ = float( hitRecord[1] ) / float( 0x7FFFFFFF );
				if( minZ < smallestZ )
				{
					smallestZ = minZ;
					grippingCube = TranslateGrip( grip, x, y, z, face );
				}
			}
		}

		hitRecord += 3 + nameCount;
	}

	return grippingCube;
}

//==================================================================================================
// A cube is in the solved state if all faces are of one color.
// Notice that there are multiple cases of the solved state,
// but relatively speaking, color adjacencies should be consistent
// in all such cases.
bool RubiksCube::IsInSolvedState( void ) const
{
	Color posXColor = subCubeMatrix[ subCubeMatrixSize - 1 ][0][0].faceColor[ POS_X ];
	Color posYColor = subCubeMatrix[0][ subCubeMatrixSize - 1 ][0].faceColor[ POS_Y ];
	Color posZColor = subCubeMatrix[0][0][ subCubeMatrixSize - 1 ].faceColor[ POS_Z ];
	Color negXColor = subCubeMatrix[0][0][0].faceColor[ NEG_X ];
	Color negYColor = subCubeMatrix[0][0][0].faceColor[ NEG_Y ];
	Color negZColor = subCubeMatrix[0][0][0].faceColor[ NEG_Z ];

	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				SubCube* subCube = &subCubeMatrix[x][y][z];

				if( x == subCubeMatrixSize - 1 && subCube->faceColor[ POS_X ] != posXColor )
					return false;
				if( y == subCubeMatrixSize - 1 && subCube->faceColor[ POS_Y ] != posYColor )
					return false;
				if( z == subCubeMatrixSize - 1 && subCube->faceColor[ POS_Z ] != posZColor )
					return false;
				if( x == 0 && subCube->faceColor[ NEG_X ] != negXColor )
					return false;
				if( y == 0 && subCube->faceColor[ NEG_Y ] != negYColor )
					return false;
				if( z == 0 && subCube->faceColor[ NEG_Z ] != negZColor )
					return false;
			}
		}
	}

	return true;
}

//==================================================================================================
// I imagine that this routine would be used by the UI as well as the AI.
// The UI would formulate the given system based on how the cube is oriented.
// The AI, however, would have to formulate it as a function of the desired perspective.
bool RubiksCube::TranslateRotation(
						const c3ga::vectorE3GA& rAxis,
						const c3ga::vectorE3GA& uAxis,
						const c3ga::vectorE3GA& fAxis,
						RelativeRotation relativeRotation, Rotation& rotation ) const
{
	c3ga::vectorE3GA axis;
	switch( relativeRotation )
	{
		case L:
		case Li:
		{
			axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, -1.0, 0.0, 0.0 );
			break;
		}
		case R:
		case Ri:
		{
			axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
			break;
		}
		case D:
		case Di:
		{
			axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, -1.0, 0.0 );
			break;
		}
		case U:
		case Ui:
		{
			axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
			break;
		}
		case B:
		case Bi:
		{
			axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, -1.0 );
			break;
		}
		case F:
		case Fi:
		{
			axis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );
			break;
		}
	}

	double angle;
	switch( relativeRotation )
	{
		case Li:
		case Ri:
		case Di:
		case Ui:
		case Bi:
		case Fi:
		{
			angle = M_PI / 2.0;
			break;
		}
		case L:
		case R:
		case D:
		case U:
		case B:
		case F:
		{
			angle = -M_PI / 2.0;
			break;
		}
	}

	axis = c3ga::gp( rAxis, axis.get_e1() ) + c3ga::gp( uAxis, axis.get_e2() ) + c3ga::gp( fAxis, axis.get_e3() );

	double epsilon = 1e-7;
	if( c3ga::norm( axis - c3ga::e1 ) < epsilon )
	{
		rotation.plane.axis = X_AXIS;
		rotation.plane.index = subCubeMatrixSize - 1;
		rotation.angle = angle;
		return true;
	}
	else if( c3ga::norm( axis + c3ga::e1 ) < epsilon )
	{
		rotation.plane.axis = X_AXIS;
		rotation.plane.index = 0;
		rotation.angle = -angle;
		return true;
	}
	else if( c3ga::norm( axis - c3ga::e2 ) < epsilon )
	{
		//...
	}

	return false;
}

// RubiksCube.cpp