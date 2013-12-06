// RubiksCube.cpp

#include "Header.h"

//==================================================================================================
const char* RubiksCube::textureFiles[ MAX_COLORS ] =
{
	"blackFace.png",
	"yellowFace.png",
	"blueFace.png",
	"redFace.png",
	"whiteFace.png",
	"greenFace.png",
	"orangeFace.png",
};

//==================================================================================================
RubiksCube::RubiksCube( int subCubeMatrixSize /*= 3*/, bool loadTextures /*= true*/ )
{
	int id = 0;

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

				subCube->faceData[ NEG_X ].color = ( x == 0 ) ? WHITE : GREY;
				subCube->faceData[ POS_X ].color = ( x == subCubeMatrixSize - 1 ) ? YELLOW : GREY;
				subCube->faceData[ NEG_Y ].color = ( y == 0 ) ? GREEN : GREY;
				subCube->faceData[ POS_Y ].color = ( y == subCubeMatrixSize - 1 ) ? BLUE : GREY;
				subCube->faceData[ NEG_Z ].color = ( z == 0 ) ? ORANGE : GREY;
				subCube->faceData[ POS_Z ].color = ( z == subCubeMatrixSize - 1 ) ? RED : GREY;

				subCube->faceData[ NEG_X ].id = ( x == 0 ) ? id++ : -1;
				subCube->faceData[ POS_X ].id = ( x == subCubeMatrixSize - 1 ) ? id++ : -1;
				subCube->faceData[ NEG_Y ].id = ( y == 0 ) ? id++ : -1;
				subCube->faceData[ POS_Y ].id = ( y == subCubeMatrixSize - 1 ) ? id++ : -1;
				subCube->faceData[ NEG_Z ].id = ( z == 0 ) ? id++ : -1;
				subCube->faceData[ POS_Z ].id = ( z == subCubeMatrixSize - 1 ) ? id++ : -1;

				subCube->x = x;
				subCube->y = y;
				subCube->z = z;
			}
		}
	}

	for( int color = 0; color < MAX_COLORS; color++ )
		textures[ color ] = GL_INVALID_VALUE;

	if( loadTextures )
		LoadTextures();
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

	UnloadTextures();
}

//==================================================================================================
void RubiksCube::LoadTextures( void )
{
	for( int color = 0; color < MAX_COLORS; color++ )
	{
		if( textures[ color ] != GL_INVALID_VALUE )
			continue;

		wxImage image;
		if( !image.LoadFile( textureFiles[ color ] ) )
			continue;

		GLuint texName;
		glGenTextures( 1, &texName );
		if( texName == GL_INVALID_VALUE )
			continue;

		glBindTexture( GL_TEXTURE_2D, texName );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

		const unsigned char* imageData = image.GetData();
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, image.GetWidth(), image.GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, imageData );

		textures[ color ] = texName;
	}
}

//==================================================================================================
void RubiksCube::UnloadTextures( void )
{
	for( int color = 0; color < MAX_COLORS; color++ )
	{
		if( textures[ color ] == GL_INVALID_VALUE )
			continue;

		glDeleteTextures( 1, &textures[ color ] );
		textures[ color ] = GL_INVALID_VALUE;
	}
}

//==================================================================================================
bool RubiksCube::Copy( const RubiksCube& rubiksCube, CopyMapFunc copyMapFunc )
{
	for( int dst_x = 0; dst_x < subCubeMatrixSize; dst_x++ )
	{
		for( int dst_y = 0; dst_y < subCubeMatrixSize; dst_y++ )
		{
			for( int dst_z = 0; dst_z < subCubeMatrixSize; dst_z++ )
			{
				SubCube* dstSubCube = &subCubeMatrix[ dst_x ][ dst_y ][ dst_z ];

				int src_x = dst_x;
				int src_y = dst_y;
				int src_z = dst_z;

				copyMapFunc( src_x, src_y, src_z );
				if( !rubiksCube.ValidMatrixCoordinates( src_x, src_y, src_z ) )
					return false;

				const SubCube* srcSubCube = &rubiksCube.subCubeMatrix[ src_x ][ src_y ][ src_z ];

				for( int face = 0; face < CUBE_FACE_COUNT; face++ )
					dstSubCube->faceData[ face ] = srcSubCube->faceData[ face ];
			}
		}
	}

	return true;
}

//==================================================================================================
/*static*/ void RubiksCube::CopyMap( int& x, int& y, int& z )
{
	// Leave the given coordinates untouched.
}

//==================================================================================================
// Notice here that we don't clear the given list.  We just append to it.
void RubiksCube::CollectSubCubes( Color* colorArray, int colorCount, SubCubeList& subCubeList ) const
{
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				const SubCube* subCube = &subCubeMatrix[x][y][z];

				int expositionCount =
					( ( x == 0 || x == subCubeMatrixSize - 1 ) ? 1 : 0 ) +
					( ( y == 0 || y == subCubeMatrixSize - 1 ) ? 1 : 0 ) +
					( ( z == 0 || z == subCubeMatrixSize - 1 ) ? 1 : 0 );

				if( expositionCount == colorCount && CubeHasAllColors( subCube, colorArray, colorCount ) )
					subCubeList.push_back( subCube );
			}
		}
	}
}

//==================================================================================================
const RubiksCube::SubCube* RubiksCube::CollectSubCube( Color* colorArray, int colorCount ) const
{
	SubCubeList subCubeList;
	CollectSubCubes( colorArray, colorCount, subCubeList );
	if( subCubeList.size() != 1 )
		return 0;
	return subCubeList.front();
}

//==================================================================================================
/*static*/ bool RubiksCube::CubeHasColor( const SubCube* subCube, Color color )
{
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
		if( subCube->faceData[ face ].color == color )
			return true;
	return false;
}

//==================================================================================================
/*static*/ bool RubiksCube::CubeHasAllColors( const SubCube* subCube, Color* colorArray, int colorCount )
{
	int colorIndex;
	for( colorIndex = 0; colorIndex < colorCount; colorIndex++ )
		if( !CubeHasColor( subCube, colorArray[ colorIndex ] ) )
			break;

	return colorIndex == colorCount ? true : false;
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
double RubiksCube::subCubeTextureCoordinates[4][2] =
{
	{ 0.0, 0.0 },
	{ 1.0, 0.0 },
	{ 1.0, 1.0 },
	{ 0.0, 1.0 },
};

//==================================================================================================
void RubiksCube::RenderSubCube( GLenum mode, const SubCube* subCube, const c3ga::evenVersor& vertexVersor, const c3ga::evenVersor& normalVersor ) const
{
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
	{
		if( mode == GL_SELECT )
			glPushName( face );

		Color color = subCube->faceData[ face ].color;
		c3ga::vectorE3GA faceColor = TranslateColor( color );

		GLuint texName = textures[ color ];
		if( texName == GL_INVALID_VALUE )
			glDisable( GL_TEXTURE_2D );
		else
		{
			glEnable( GL_TEXTURE_2D );
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
			glBindTexture( GL_TEXTURE_2D, texName );
		}
		
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
			glTexCoord2dv( subCubeTextureCoordinates[ index ] );
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
		case YELLOW:	colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 0.0 ); break;
		case BLUE:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 ); break;
		case RED:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 ); break;
		case WHITE:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 ); break;
		case GREEN:		colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.3, 0.0 ); break;
		case ORANGE:	colorVector.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.3, 0.0 ); break;
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
/*static*/ RubiksCube::Face RubiksCube::TranslateNormal( const c3ga::vectorE3GA& unitNormal )
{
	Face face = CUBE_FACE_COUNT;

	double epsilon = 1e-7;
	if( fabs( unitNormal.get_e1() + 1.0 ) < epsilon )
		face = NEG_X;
	else if( fabs( unitNormal.get_e1() - 1.0 ) < epsilon )
		face = POS_X;
	else if( fabs( unitNormal.get_e2() + 1.0 ) < epsilon )
		face = NEG_Y;
	else if( fabs( unitNormal.get_e2() - 1.0 ) < epsilon )
		face = POS_Y;
	else if( fabs( unitNormal.get_e3() + 1.0 ) < epsilon )
		face = NEG_Z;
	else if( fabs( unitNormal.get_e3() - 1.0 ) < epsilon )
		face = POS_Z;
	
	return face;
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
				case X_AXIS: SwapFaces( subCube, NEG_Z, POS_Z ); break;
				case Y_AXIS: SwapFaces( subCube, NEG_X, POS_X ); break;
				case Z_AXIS: SwapFaces( subCube, NEG_Y, POS_Y ); break;
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
				case X_AXIS: SwapFaces( subCube, NEG_Y, NEG_Z ); SwapFaces( subCube, POS_Y, POS_Z ); break;
				case Y_AXIS: SwapFaces( subCube, NEG_Z, NEG_X ); SwapFaces( subCube, POS_Z, POS_X ); break;
				case Z_AXIS: SwapFaces( subCube, NEG_X, NEG_Y ); SwapFaces( subCube, POS_X, POS_Y ); break;
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
/*static*/ void RubiksCube::SwapFaces( SubCube* subCube, int i, int j )
{
	SwapFaces( subCube->faceData[i], subCube->faceData[j] );
}

//==================================================================================================
/*static*/ void RubiksCube::SwapFaces( SubCube::FaceData& faceData0, SubCube::FaceData& faceData1 )
{
	SubCube::FaceData tempFaceData = faceData0;
	faceData0 = faceData1;
	faceData1 = tempFaceData;
}

//==================================================================================================
/*static*/ void RubiksCube::SwapSubCubes( SubCube* subCube0, SubCube* subCube1 )
{
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
		SwapFaces( subCube0->faceData[ face ], subCube1->faceData[ face ] );
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
			if( subCube->faceData[ face ].color != GREY )
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
	Color posXColor = subCubeMatrix[ subCubeMatrixSize - 1 ][0][0].faceData[ POS_X ].color;
	Color posYColor = subCubeMatrix[0][ subCubeMatrixSize - 1 ][0].faceData[ POS_Y ].color;
	Color posZColor = subCubeMatrix[0][0][ subCubeMatrixSize - 1 ].faceData[ POS_Z ].color;
	Color negXColor = subCubeMatrix[0][0][0].faceData[ NEG_X ].color;
	Color negYColor = subCubeMatrix[0][0][0].faceData[ NEG_Y ].color;
	Color negZColor = subCubeMatrix[0][0][0].faceData[ NEG_Z ].color;

	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				SubCube* subCube = &subCubeMatrix[x][y][z];

				if( x == subCubeMatrixSize - 1 && subCube->faceData[ POS_X ].color != posXColor )
					return false;
				if( y == subCubeMatrixSize - 1 && subCube->faceData[ POS_Y ].color != posYColor )
					return false;
				if( z == subCubeMatrixSize - 1 && subCube->faceData[ POS_Z ].color != posZColor )
					return false;
				if( x == 0 && subCube->faceData[ NEG_X ].color != negXColor )
					return false;
				if( y == 0 && subCube->faceData[ NEG_Y ].color != negYColor )
					return false;
				if( z == 0 && subCube->faceData[ NEG_Z ].color != negZColor )
					return false;
			}
		}
	}

	return true;
}

//==================================================================================================
// This routine won't catch all possible ways that a given sequence can be compressed,
// but we may be able to catch a few cases here.  For now, there are only a few cases
// that we catch.
/*static*/ void RubiksCube::CompressRotationSequence( RotationSequence& rotationSequence )
{
	double epsilon = 1e-7;

	// One case we don't catch here is, for example, to turn the sequence {U:0,U:2} into
	// {Ui:1}, but to do this, you would have to then adjust all rotations after this in
	// the sequence, and this only applies to the 3x3x3, but the idea has its equivilants
	// with the NxNxN cube.

	bool optimizationFound = false;
	do
	{
		optimizationFound = false;

		for( RotationSequence::iterator iter0 = rotationSequence.begin(); iter0 != rotationSequence.end(); iter0++ )
		{
			Rotation rotation0 = *iter0;
			if( rotation0.angle == 0.0 )
			{
				rotationSequence.erase( iter0 );
				optimizationFound = true;
				break;
			}

			if( rotation0.angle > M_PI )
			{
				do
				{
					rotation0.angle -= 2.0 * M_PI;
				}
				while( rotation0.angle > M_PI );
				*iter0 = rotation0;
			}

			if( rotation0.angle < -M_PI )
			{
				do
				{
					rotation0.angle += 2.0 * M_PI;
				}
				while( rotation0.angle < -M_PI );
				*iter0 = rotation0;
			}

			RotationSequence::iterator iter1 = iter0;
			iter1++;

			if( iter1 != rotationSequence.end() )
			{
				Rotation rotation1 = *iter1;

				if( rotation0.plane.axis == rotation1.plane.axis && rotation0.plane.index == rotation1.plane.index )
				{
					// Adjacent rotations of opposite angles cancel one another out.
					if( fabs( rotation0.angle + rotation1.angle ) < epsilon )
					{
						rotationSequence.erase( iter0 );
						rotationSequence.erase( iter1 );
						optimizationFound = true;
						break;
					}

					// This isn't quite an optimization, but it does compress the sequence a bit.
					rotation0.angle += rotation1.angle;
					rotationSequence.erase( iter1 );
					*iter0 = rotation0;
					optimizationFound = true;
					break;
				}
			}
		}
	}
	while( optimizationFound );
}

//==================================================================================================
// Notice here that we do not clear the given list before appending to it.
bool RubiksCube::TranslateRotationSequence( const Perspective& perspective, const RelativeRotationSequence& relativeRotationSequence, RotationSequence& rotationSequence ) const
{
	for( RelativeRotationSequence::const_iterator iter = relativeRotationSequence.begin(); iter != relativeRotationSequence.end(); iter++ )
	{
		RelativeRotation relativeRotation = *iter;
		Rotation rotation;
		if( !TranslateRotation( perspective, relativeRotation, rotation ) )
			return false;

		rotationSequence.push_back( rotation );
	}

	return true;
}

//==================================================================================================
// I imagine that this routine would be used by the UI as well as the AI.
// The UI would formulate the given system based on how the cube is oriented.
// The AI, however, would have to formulate it as a function of the desired perspective.
bool RubiksCube::TranslateRotation( const Perspective& perspective, const RelativeRotation& relativeRotation, Rotation& rotation ) const
{
	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	wxASSERT( 0 <= relativeRotation.planeIndex && relativeRotation.planeIndex < subCubeMatrixSize );
	if( !( 0 <= relativeRotation.planeIndex && relativeRotation.planeIndex < subCubeMatrixSize ) )
		return false;

	c3ga::vectorE3GA axis;
	switch( relativeRotation.type )
	{
		case RelativeRotation::L:
		case RelativeRotation::Li:	axis = -xAxis;	break;
		case RelativeRotation::R:
		case RelativeRotation::Ri:	axis = xAxis;	break;
		case RelativeRotation::D:
		case RelativeRotation::Di:	axis = -yAxis;	break;
		case RelativeRotation::U:
		case RelativeRotation::Ui:	axis = yAxis;	break;
		case RelativeRotation::B:
		case RelativeRotation::Bi:	axis = -zAxis;	break;
		case RelativeRotation::F:
		case RelativeRotation::Fi:	axis = zAxis;	break;
	}

	double angle;
	switch( relativeRotation.type )
	{
		case RelativeRotation::Li:
		case RelativeRotation::Ri:
		case RelativeRotation::Di:
		case RelativeRotation::Ui:
		case RelativeRotation::Bi:
		case RelativeRotation::Fi:	angle = M_PI / 2.0;		break;
		case RelativeRotation::L:
		case RelativeRotation::R:
		case RelativeRotation::D:
		case RelativeRotation::U:
		case RelativeRotation::B:
		case RelativeRotation::F:	angle = -M_PI / 2.0;	break;
	}

	axis =
		c3ga::gp( perspective.rAxis, axis.get_e1() ) +
		c3ga::gp( perspective.uAxis, axis.get_e2() ) +
		c3ga::gp( perspective.fAxis, axis.get_e3() );

	double epsilon = 1e-7;
	if( c3ga::norm( axis - xAxis ) < epsilon )
	{
		rotation.plane.axis = X_AXIS;
		rotation.plane.index = subCubeMatrixSize - 1 - relativeRotation.planeIndex;
		rotation.angle = angle;
		return true;
	}
	else if( c3ga::norm( axis + xAxis ) < epsilon )
	{
		rotation.plane.axis = X_AXIS;
		rotation.plane.index = relativeRotation.planeIndex;
		rotation.angle = -angle;
		return true;
	}
	else if( c3ga::norm( axis - yAxis ) < epsilon )
	{
		rotation.plane.axis = Y_AXIS;
		rotation.plane.index = subCubeMatrixSize - 1 - relativeRotation.planeIndex;
		rotation.angle = angle;
		return true;
	}
	else if( c3ga::norm( axis + yAxis ) < epsilon )
	{
		rotation.plane.axis = Y_AXIS;
		rotation.plane.index = relativeRotation.planeIndex;
		rotation.angle = -angle;
		return true;
	}
	else if( c3ga::norm( axis - zAxis ) < epsilon )
	{
		rotation.plane.axis = Z_AXIS;
		rotation.plane.index = subCubeMatrixSize - 1 - relativeRotation.planeIndex;
		rotation.angle = angle;
		return true;
	}
	else if( c3ga::norm( axis + zAxis ) < epsilon )
	{
		rotation.plane.axis = Z_AXIS;
		rotation.plane.index = relativeRotation.planeIndex;
		rotation.angle = -angle;
		return true;
	}

	return false;
}

//==================================================================================================
bool RubiksCube::ParseRelativeRotationSequenceString( const std::string& relativeRotationSequenceString, RelativeRotationSequence& relativeRotationSequence ) const
{
	boost::char_separator< char > separator( " ", "," );
	typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;
	Tokenizer tokenizer( relativeRotationSequenceString, separator );

	for( Tokenizer::iterator iter = tokenizer.begin(); iter != tokenizer.end(); iter++ )
	{
		std::string token = *iter;
		if( token == "," )
			continue;

		RubiksCube::RelativeRotation relativeRotation;
		if( TranslateRelativeRotation( token, relativeRotation ) )
			relativeRotationSequence.push_back( relativeRotation );
		else
			return false;
	}

	return true;
}

//==================================================================================================
bool RubiksCube::TranslateRelativeRotation( const std::string& relativeRotationString, RelativeRotation& relativeRotation ) const
{
	relativeRotation.planeIndex = 0;

	boost::char_separator< char > separator( " ", ":" );
	typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;
	Tokenizer tokenizer( relativeRotationString, separator );

	// The first token should be the relative face.
	Tokenizer::iterator iter = tokenizer.begin();
	std::string token = *iter;
	if( token == "L" )
		relativeRotation.type = RubiksCube::RelativeRotation::L;
	else if( token == "R" )
		relativeRotation.type = RubiksCube::RelativeRotation::R;
	else if( token == "D" )
		relativeRotation.type = RubiksCube::RelativeRotation::D;
	else if( token == "U" )
		relativeRotation.type = RubiksCube::RelativeRotation::U;
	else if( token == "B" )
		relativeRotation.type = RubiksCube::RelativeRotation::B;
	else if( token == "F" )
		relativeRotation.type = RubiksCube::RelativeRotation::F;
	else if( token == "Li" )
		relativeRotation.type = RubiksCube::RelativeRotation::Li;
	else if( token == "Ri" )
		relativeRotation.type = RubiksCube::RelativeRotation::Ri;
	else if( token == "Di" )
		relativeRotation.type = RubiksCube::RelativeRotation::Di;
	else if( token == "Ui" )
		relativeRotation.type = RubiksCube::RelativeRotation::Ui;
	else if( token == "Bi" )
		relativeRotation.type = RubiksCube::RelativeRotation::Bi;
	else if( token == "Fi" )
		relativeRotation.type = RubiksCube::RelativeRotation::Fi;
	else
		return false;

	// The next token, if any, should be a colon.
	if( ++iter == tokenizer.end() )
		return true;
	token = *iter;
	if( token != ":" )
		return false;

	// The last token should be the plane index.
	if( ++iter == tokenizer.end() )
		return false;
	token = *iter;
	relativeRotation.planeIndex = atoi( token.c_str() );
	if( relativeRotation.planeIndex < 0 || relativeRotation.planeIndex > subCubeMatrixSize - 1 )
		return false;

	return true;
}

//==================================================================================================
const RubiksCube::SubCube* RubiksCube::Matrix( int x, int y, int z ) const
{
	if( !ValidMatrixCoordinates( x, y, z ) )
		return 0;

	return &subCubeMatrix[x][y][z];
}

//==================================================================================================
bool RubiksCube::ValidMatrixCoordinates( int x, int y, int z ) const
{
	if( x < 0 || y < 0 || z < 0 )
		return false;
	if( x > subCubeMatrixSize - 1 )
		return false;
	if( y > subCubeMatrixSize - 1 )
		return false;
	if( z > subCubeMatrixSize - 1 )
		return false;
	return true;
}

//==================================================================================================
bool RubiksCube::SaveToFile( const wxString& file ) const
{
	return SaveToFile( this, file );
}

//==================================================================================================
/*static*/ bool RubiksCube::SaveToFile( const RubiksCube* rubiksCube, const wxString& file )
{
	bool success = false;
	
	do
	{
		wxXmlNode* xmlRoot = new wxXmlNode( 0, wxXML_ELEMENT_NODE, "RubiksCube" );

		int size = rubiksCube->subCubeMatrixSize;
		xmlRoot->AddAttribute( "size", wxString::Format( "%d", size ) );

		if( !rubiksCube->SaveToXml( xmlRoot ) )
			break;

		wxXmlDocument xmlDocument;
		xmlDocument.SetRoot( xmlRoot );

		if( !xmlDocument.Save( file ) )
			break;

		success = true;
	}
	while( false );

	return success;
}

//==================================================================================================
/*static*/ RubiksCube* RubiksCube::LoadFromFile( const wxString& file )
{
	bool success = false;
	RubiksCube* rubiksCube = 0;

	do
	{
		wxXmlDocument xmlDocument;
		if( !xmlDocument.Load( file ) )
			break;

		wxXmlNode* xmlRoot = xmlDocument.GetRoot();
		if( xmlRoot->GetName() != "RubiksCube" )
			break;

		wxString sizeString;
		if( !xmlRoot->GetAttribute( "size", &sizeString ) )
			break;

		long size;
		if( !sizeString.ToLong( &size ) )
			break;

		rubiksCube = new RubiksCube( int( size ) );
		if( !rubiksCube->LoadFromXml( xmlRoot ) )
			break;

		success = true;
	}
	while( false );

	if( !success )
	{
		delete rubiksCube;
		rubiksCube = 0;
	}

	return rubiksCube;
}

//==================================================================================================
bool RubiksCube::SaveToXml( wxXmlNode* xmlNode ) const
{
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				const SubCube* subCube = &subCubeMatrix[x][y][z];

				wxXmlNode* xmlSubCube = new wxXmlNode( xmlNode, wxXML_ELEMENT_NODE, "SubCube" );

				xmlSubCube->AddAttribute( "x", wxString::Format( "%d", x ) );
				xmlSubCube->AddAttribute( "y", wxString::Format( "%d", y ) );
				xmlSubCube->AddAttribute( "z", wxString::Format( "%d", z ) );
				
				if( !SaveIntegerToXml( xmlSubCube, "neg_x_color", subCube->faceData[ NEG_X ].color ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "pos_x_color", subCube->faceData[ POS_X ].color ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "neg_y_color", subCube->faceData[ NEG_Y ].color ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "pos_y_color", subCube->faceData[ POS_Y ].color ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "neg_z_color", subCube->faceData[ NEG_Z ].color ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "pos_z_color", subCube->faceData[ POS_Z ].color ) )
					return false;

				if( !SaveIntegerToXml( xmlSubCube, "neg_x_id", subCube->faceData[ NEG_X ].id ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "pos_x_id", subCube->faceData[ POS_X ].id ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "neg_y_id", subCube->faceData[ NEG_Y ].id ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "pos_y_id", subCube->faceData[ POS_Y ].id ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "neg_z_id", subCube->faceData[ NEG_Z ].id ) )
					return false;
				if( !SaveIntegerToXml( xmlSubCube, "pos_z_id", subCube->faceData[ POS_Z ].id ) )
					return false;
			}
		}
	}

	return true;
}

//==================================================================================================
bool RubiksCube::LoadFromXml( const wxXmlNode* xmlNode )
{
	for( const wxXmlNode* xmlSubCube = xmlNode->GetChildren(); xmlSubCube; xmlSubCube = xmlSubCube->GetNext() )
	{
		if( xmlSubCube->GetName() != "SubCube" )
			continue;

		wxString xString, yString, zString;
		if( !( xmlSubCube->GetAttribute( "x", &xString ) && xmlSubCube->GetAttribute( "y", &yString ) && xmlSubCube->GetAttribute( "z", &zString ) ) )
			return false;

		long xLong = -1, yLong = -1, zLong = -1;
		if( !( xString.ToLong( &xLong ) && yString.ToLong( &yLong ) && zString.ToLong( &zLong ) ) )
			return false;

		int x = int( xLong );
		int y = int( yLong );
		int z = int( zLong );
		if( !ValidMatrixCoordinates( x, y, z ) )
			return false;

		SubCube* subCube = &subCubeMatrix[x][y][z];

		int color;
		
		if( !LoadIntegerFromXml( xmlSubCube, "neg_x_color", color ) )
			return false;
		subCube->faceData[ NEG_X ].color = Color( color );

		if( !LoadIntegerFromXml( xmlSubCube, "pos_x_color", color ) )
			return false;
		subCube->faceData[ POS_X ].color = Color( color );

		if( !LoadIntegerFromXml( xmlSubCube, "neg_y_color", color ) )
			return false;
		subCube->faceData[ NEG_Y ].color = Color( color );

		if( !LoadIntegerFromXml( xmlSubCube, "pos_y_color", color ) )
			return false;
		subCube->faceData[ POS_Y ].color = Color( color );

		if( !LoadIntegerFromXml( xmlSubCube, "neg_z_color", color ) )
			return false;
		subCube->faceData[ NEG_Z ].color = Color( color );

		if( !LoadIntegerFromXml( xmlSubCube, "pos_z_color", color ) )
			return false;
		subCube->faceData[ POS_Z ].color = Color( color );

		if( !LoadIntegerFromXml( xmlSubCube, "neg_x_id", subCube->faceData[ NEG_X ].id ) )
			return false;
		if( !LoadIntegerFromXml( xmlSubCube, "pos_x_id", subCube->faceData[ POS_X ].id ) )
			return false;
		if( !LoadIntegerFromXml( xmlSubCube, "neg_y_id", subCube->faceData[ NEG_Y ].id ) )
			return false;
		if( !LoadIntegerFromXml( xmlSubCube, "pos_y_id", subCube->faceData[ POS_Y ].id ) )
			return false;
		if( !LoadIntegerFromXml( xmlSubCube, "neg_z_id", subCube->faceData[ NEG_Z ].id ) )
			return false;
		if( !LoadIntegerFromXml( xmlSubCube, "pos_z_id", subCube->faceData[ POS_Z ].id ) )
			return false;
	}

	return true;
}

//==================================================================================================
/*static*/ bool RubiksCube::SaveIntegerToXml( wxXmlNode* xmlNode, const wxString& name, int integer )
{
	wxString integerString = wxString::Format( "%d", integer );
	xmlNode->AddAttribute( name, integerString );
	return true;
}

//==================================================================================================
/*static*/ bool RubiksCube::LoadIntegerFromXml( const wxXmlNode* xmlNode, const wxString& name, int& integer )
{
	wxString integerString;
	if( !xmlNode->GetAttribute( name, &integerString ) )
		return false;

	long integerLong;
	if( !integerString.ToLong( &integerLong ) )
		return false;

	integer = int( integerLong );
	return true;
}

//==================================================================================================
void RubiksCube::Scramble( int seed, int rotationCount, RotationSequence* rotationSequence /*= 0*/, bool apply /*= true*/ )
{
	srand( seed );

	RotationSequence localRotationSequence;
	if( !rotationSequence )
		rotationSequence = &localRotationSequence;

	for( int count = 0; count < rotationCount; count++ )
	{
		Rotation rotation;
		rotation.plane.index = rand() % subCubeMatrixSize;
		rotation.plane.axis = Axis( rand() % 3 );
		rotation.angle = double( 1 + rand() % 3 ) * M_PI / 2.0;
		rotationSequence->push_back( rotation );
	}

	if( apply )
		ApplySequence( *rotationSequence );
}

//==================================================================================================
Solver* RubiksCube::MakeSolver( void ) const
{
	Solver* solver = 0;

	switch( subCubeMatrixSize )
	{
		case 1:
		{
			// It's already solved.  ;)
			break;
		}
		case 2:
		{
			solver = new SolverForCase2();
			break;
		}
		case 3:
		{
			solver = new SolverForCase3();
			break;
		}
		default:
		{
			solver = new SolverForCaseGreaterThan3();
			break;
		}
	}

	return solver;
}

//==================================================================================================
RubiksCube::RelativeRotation::RelativeRotation( Type type /*= U*/, int planeIndex /*= 0*/ )
{
	this->type = type;
	this->planeIndex = planeIndex;
}

// RubiksCube.cpp