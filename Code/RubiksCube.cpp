// RubiksCube.cpp

#include "Header.h"

// TODO: It would be awesome to let the user load their own textures for the 6 faces of the cube.
//       This adds an interesting challenge to the cube as the center pieces then require orientation.

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
	enforceBandaging = false;

	int faceId = 0;
	int subCubeId = 0;

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

				subCube->faceData[ NEG_X ].color = ( x == 0 ) ? TranslateFaceColor( NEG_X ) : GREY;
				subCube->faceData[ POS_X ].color = ( x == subCubeMatrixSize - 1 ) ? TranslateFaceColor( POS_X ) : GREY;
				subCube->faceData[ NEG_Y ].color = ( y == 0 ) ? TranslateFaceColor( NEG_Y ) : GREY;
				subCube->faceData[ POS_Y ].color = ( y == subCubeMatrixSize - 1 ) ? TranslateFaceColor( POS_Y ) : GREY;
				subCube->faceData[ NEG_Z ].color = ( z == 0 ) ? TranslateFaceColor( NEG_Z ) : GREY;
				subCube->faceData[ POS_Z ].color = ( z == subCubeMatrixSize - 1 ) ? TranslateFaceColor( POS_Z ) : GREY;

				subCube->faceData[ NEG_X ].id = ( x == 0 ) ? faceId++ : -1;
				subCube->faceData[ POS_X ].id = ( x == subCubeMatrixSize - 1 ) ? faceId++ : -1;
				subCube->faceData[ NEG_Y ].id = ( y == 0 ) ? faceId++ : -1;
				subCube->faceData[ POS_Y ].id = ( y == subCubeMatrixSize - 1 ) ? faceId++ : -1;
				subCube->faceData[ NEG_Z ].id = ( z == 0 ) ? faceId++ : -1;
				subCube->faceData[ POS_Z ].id = ( z == subCubeMatrixSize - 1 ) ? faceId++ : -1;

				subCube->coords.x = x;
				subCube->coords.y = y;
				subCube->coords.z = z;

				subCube->bandageCoords.x = -1;
				subCube->bandageCoords.y = -1;
				subCube->bandageCoords.z = -1;

				subCube->bandaged = false;

				subCube->id = subCubeId++;
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

		wxImage* image = wxGetApp().LoadTextureResource( textureFiles[ color ] );
		if( !image )
			continue;

		GLuint texName;
		glGenTextures( 1, &texName );
		if( texName != GL_INVALID_VALUE )
		{
			glBindTexture( GL_TEXTURE_2D, texName );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

			const unsigned char* imageData = image->GetData();
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, image->GetWidth(), image->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, imageData );

			textures[ color ] = texName;
		}
		
		delete image;
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

				Coordinates src;
				src.x = dst_x;
				src.y = dst_y;
				src.z = dst_z;

				copyMapFunc( src );
				const SubCube* srcSubCube = rubiksCube.Matrix( src );
				if( !srcSubCube )
					return false;

				for( int face = 0; face < CUBE_FACE_COUNT; face++ )
					dstSubCube->faceData[ face ] = srcSubCube->faceData[ face ];

				dstSubCube->id = srcSubCube->id;
			}
		}
	}

	return true;
}

//==================================================================================================
/*static*/ void RubiksCube::CopyMap( Coordinates& coords )
{
	// Leave the given coordinates untouched.
}

//==================================================================================================
// Notice here that we don't clear the given vector.  We just append to it.
void RubiksCube::CollectSubCubes( Color* colorArray, int colorCount, SubCubeVector& subCubeVector ) const
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
					subCubeVector.push_back( subCube );
			}
		}
	}
}

//==================================================================================================
const RubiksCube::SubCube* RubiksCube::CollectSubCube( Color* colorArray, int colorCount ) const
{
	SubCubeVector subCubeVector;
	CollectSubCubes( colorArray, colorCount, subCubeVector );
	if( subCubeVector.size() != 1 )
		return 0;
	return subCubeVector[0];
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
// We expect the given color array to have room for at least 3 colors.
/*static*/ int RubiksCube::ExposedColors( const SubCube* subCube, Color* colorArray )
{
	int colorCount = 0;
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
	{
		Color color = subCube->faceData[ face ].color;
		if( color != GREY )
			colorArray[ colorCount++ ] = color;
	}
	return colorCount;
}

//==================================================================================================
// We expect the given face array to have room for at least 3 faces.
/*static*/ int RubiksCube::ExposedFaces( const SubCube* subCube, Face* faceArray )
{
	int faceCount = 0;
	for( int face = 0; face < CUBE_FACE_COUNT; face++ )
	{
		Color color = subCube->faceData[ face ].color;
		if( color != GREY )
			faceArray[ faceCount++ ] = ( RubiksCube::Face )face;
	}
	return faceCount;
}

//==================================================================================================
int RubiksCube::SubCubeMatrixSize( void ) const
{
	return subCubeMatrixSize;
}

//==================================================================================================
void RubiksCube::RenderSubCube( GLenum mode, int x, int y, int z,
										const RotationSequence& rotationSequence, const Size& size,
										int* selectedFaceId,
										const RubiksCube* comparativeRubiksCube,
										bool highlightInvariants ) const
{
	double t;
	c3ga::vectorE3GA subCubeCenter;

	t = double( x ) / double( subCubeMatrixSize - 1 );
	subCubeCenter.set_e1( size.cubeWidthHeightAndDepth * ( t - 0.5 ) );

	t = double( y ) / double( subCubeMatrixSize - 1 );
	subCubeCenter.set_e2( size.cubeWidthHeightAndDepth * ( t - 0.5 ) );

	t = double( z ) / double( subCubeMatrixSize - 1 );
	subCubeCenter.set_e3( size.cubeWidthHeightAndDepth * ( t - 0.5 ) );

	if( mode == GL_SELECT )
	{
		glPushName( x );
		glPushName( y );
		glPushName( z );
	}

	c3ga::rotorE3GA rotor( c3ga::rotorE3GA::coord_scalar_e1e2_e2e3_e3e1, 1.0, 0.0, 0.0, 0.0 );

	for( RotationSequence::const_iterator iter = rotationSequence.cbegin(); iter != rotationSequence.cend(); iter++ )
	{
		const Rotation& rotation = *iter;

		if( ( rotation.plane.axis == X_AXIS && x == rotation.plane.index ) ||
			( rotation.plane.axis == Y_AXIS && y == rotation.plane.index ) ||
			( rotation.plane.axis == Z_AXIS && z == rotation.plane.index ) )
		{
			c3ga::vectorE3GA rotationAxis = TranslateAxis( rotation.plane.axis );
			double rotationAngle = rotation.angle;
			c3ga::bivectorE3GA rotationBlade;
			rotationBlade.set( c3ga::lc( rotationAxis, c3ga::trivectorE3GA( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 ) ) );
			rotor = c3ga::exp( rotationBlade * -0.5 * rotationAngle );
			break;
		}
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
	const SubCube* comparativeSubCube = 0;
	if( mode == GL_RENDER && comparativeRubiksCube && comparativeRubiksCube->ValidMatrixCoordinates( subCube->coords ) )
		comparativeSubCube = comparativeRubiksCube->Matrix( subCube->coords );
	RenderSubCube( mode, subCube, vertexVersor, normalVersor, selectedFaceId, comparativeSubCube, highlightInvariants );

	if( mode == GL_SELECT )
	{
		glPopName();
		glPopName();
		glPopName();
	}
}

//==================================================================================================
void RubiksCube::ClearBandaging( void )
{
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				SubCube* subCube = &subCubeMatrix[x][y][z];
				subCube->bandageCoords.x = -1;
				subCube->bandageCoords.y = -1;
				subCube->bandageCoords.z = -1;
				subCube->bandaged = false;
			}
		}
	}
}

//==================================================================================================
RubiksCube::SubCube* RubiksCube::SubCubeBandageRepresentative( SubCube* subCube )
{
	const SubCube* subCubeRep = const_cast< const RubiksCube* >( this )->SubCubeBandageRepresentative( subCube );
	return const_cast< SubCube* >( subCubeRep );
}

//==================================================================================================
const RubiksCube::SubCube* RubiksCube::SubCubeBandageRepresentative( const SubCube* subCube ) const
{
	const SubCube* subCubeRep = subCube;
	while( subCubeRep->bandageCoords.x >= 0 )
		subCubeRep = Matrix( subCubeRep->bandageCoords );
	
	while( subCube != subCubeRep )
	{
		subCube->bandageCoords = subCubeRep->coords;
		subCube = Matrix( subCube->bandageCoords );
	}

	return subCubeRep;
}

//==================================================================================================
void RubiksCube::CollectBandagedCubies( SubCube* givenSubCube, SubCubeVector& subCubeVector )
{
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				SubCube* subCube = &subCubeMatrix[x][y][z];
				if( SubCubeBandageRepresentative( subCube ) == SubCubeBandageRepresentative( givenSubCube ) )
					subCubeVector.push_back( subCube );
			}
		}
	}
}

//==================================================================================================
void RubiksCube::BandageCubies( SubCube* subCubeA, SubCube* subCubeB )
{
	SubCube* subCubeRepA = SubCubeBandageRepresentative( subCubeA );
	SubCube* subCubeRepB = SubCubeBandageRepresentative( subCubeB );

	if( subCubeRepA == subCubeRepB )
		return;

	// This is arbitrarily done.  We could do it vice-versa.
	subCubeRepA->bandageCoords = subCubeRepB->coords;

	if( subCubeA->bandaged == subCubeB->bandaged )
	{
		wxColourDialog colorDialog( wxGetApp().frame );
		if( wxID_OK != colorDialog.ShowModal() )
			return;

		wxColour color = colorDialog.GetColourData().GetColour();
		c3ga::vectorE3GA bandageColor;
		bandageColor.set_e1( double( color.Red() ) / 255.0 );
		bandageColor.set_e2( double( color.Green() ) / 255.0 );
		bandageColor.set_e3( double( color.Blue() ) / 255.0 );

		if( !subCubeA->bandaged )
		{
			subCubeA->bandageColor = bandageColor;
			subCubeB->bandageColor = bandageColor;
		}
		else
		{
			SubCubeVector subCubeVector;
			CollectBandagedCubies( subCubeA, subCubeVector );
			for( int i = 0; i < ( int )subCubeVector.size(); i++ )
				*const_cast< c3ga::vectorE3GA* >( &subCubeVector[i]->bandageColor ) = bandageColor;
		}
	}
	else if( subCubeA->bandaged )
		subCubeB->bandageColor = subCubeA->bandageColor;
	else if( subCubeB->bandaged )
		subCubeA->bandageColor = subCubeB->bandageColor;
	else
	{
		wxASSERT( false );
	}

	subCubeA->bandaged = true;
	subCubeB->bandaged = true;
}

//==================================================================================================
void RubiksCube::CollectCubiesInPlaneOfRotation( const Rotation& rotation, SubCubeVector& subCubeVector ) const
{
	subCubeVector.clear();

	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			// Skip internal cubies.
			if( rotation.plane.index > 0 && rotation.plane.index < subCubeMatrixSize - 1 )
				if( x > 0 && x < subCubeMatrixSize - 1 && y > 0 && y < subCubeMatrixSize - 1 )
					continue;

			const SubCube* subCube = SubCubeIndexPlane( rotation.plane, x, y );
			subCubeVector.push_back( subCube );
		}
	}
}

//==================================================================================================
bool RubiksCube::GenerateRotationSequenceForBandaging( const Rotation& rotation, RotationSequence& rotationSequence ) const
{
	rotationSequence.clear();
	rotationSequence.push_back( rotation );

	if( enforceBandaging )
	{
		SubCubeVector subCubeVector;
		CollectCubiesInPlaneOfRotation( rotation, subCubeVector );

		std::set< const SubCube* > involvedBandageSets;
		for( int i = 0; i < ( int )subCubeVector.size(); i++ )
			involvedBandageSets.insert( SubCubeBandageRepresentative( subCubeVector[i] ) );

		for( int index = 0; index < ( int )subCubeMatrixSize; index++ )
		{
			if( index == rotation.plane.index )
				continue;

			Rotation adjacentRotation = rotation;
			adjacentRotation.plane.index = index;
			CollectCubiesInPlaneOfRotation( adjacentRotation, subCubeVector );

			for( int i = 0; i < ( int )subCubeVector.size(); i++ )
			{
				std::set< const SubCube* >::iterator iter = involvedBandageSets.find( SubCubeBandageRepresentative( subCubeVector[i] ) );
				if( iter != involvedBandageSets.end() )
					rotationSequence.push_back( adjacentRotation );
			}
		}
	}

	return true;
}

//==================================================================================================
void RubiksCube::Render( GLenum mode, const Rotation& rotation, const Size& size,
										int* selectedFaceId /*= 0*/,
										const RubiksCube* comparativeRubiksCube /*= 0*/,
										bool highlightInvariants /*= false*/ ) const
{
	RotationSequence rotationSequence;
	GenerateRotationSequenceForBandaging( rotation, rotationSequence );
	Render( mode, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
}

//==================================================================================================
void RubiksCube::Render( GLenum mode, const RotationSequence& rotationSequence, const Size& size,
										int* selectedFaceId /*= 0*/,
										const RubiksCube* comparativeRubiksCube /*= 0*/,
										bool highlightInvariants /*= false*/ ) const
{
	for( int i = 1; i < subCubeMatrixSize - 1; i++ )
	{
		for( int j = 1; j < subCubeMatrixSize - 1; j++ )
		{
			RenderSubCube( mode, i, j, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
			RenderSubCube( mode, 0, i, j, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
			RenderSubCube( mode, j, 0, i, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );

			RenderSubCube( mode, i, j, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
			RenderSubCube( mode, subCubeMatrixSize - 1, i, j, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
			RenderSubCube( mode, j, subCubeMatrixSize - 1, i, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		}
	}

	for( int i = 1; i < subCubeMatrixSize - 1; i++ )
	{
		RenderSubCube( mode, i, 0, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, i, subCubeMatrixSize - 1, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, i, 0, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, i, subCubeMatrixSize - 1, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );

		RenderSubCube( mode, 0, i, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, subCubeMatrixSize - 1, i, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, 0, i, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, subCubeMatrixSize - 1, i, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );

		RenderSubCube( mode, 0, 0, i, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, subCubeMatrixSize - 1, 0, i, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, 0, subCubeMatrixSize - 1, i, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
		RenderSubCube( mode, subCubeMatrixSize - 1, subCubeMatrixSize - 1, i, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	}

	RenderSubCube( mode, 0, 0, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, subCubeMatrixSize - 1, 0, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, 0, subCubeMatrixSize - 1, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, subCubeMatrixSize - 1, subCubeMatrixSize - 1, 0, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, 0, 0, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, subCubeMatrixSize - 1, 0, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, 0, subCubeMatrixSize - 1, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
	RenderSubCube( mode, subCubeMatrixSize - 1, subCubeMatrixSize - 1, subCubeMatrixSize - 1, rotationSequence, size, selectedFaceId, comparativeRubiksCube, highlightInvariants );
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
void RubiksCube::RenderSubCube( GLenum mode, const SubCube* subCube,
												const c3ga::evenVersor& vertexVersor,
												const c3ga::evenVersor& normalVersor,
												int* selectedFaceId,
												const SubCube* comparativeSubCube,
												bool highlightInvariants ) const
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
		
		c3ga::vectorE3GA faceNormal = TranslateNormal( Face( face ) );
		faceNormal = c3ga::applyUnitVersor( normalVersor, faceNormal );
		if( GL_FALSE == glIsEnabled( GL_LIGHTING ) )
			glColor3d( faceColor.get_e1(), faceColor.get_e2(), faceColor.get_e3() );
		else
		{	
			glNormal3f( faceNormal.get_e1(), faceNormal.get_e2(), faceNormal.get_e3() );
			GLfloat ambientDiffuse[] = { ( GLfloat )faceColor.get_e1(), ( GLfloat )faceColor.get_e2(), ( GLfloat )faceColor.get_e3(), 1.f };
			glMaterialfv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientDiffuse );
		}

		c3ga::dualSphere dualPoint;
		dualPoint.set( c3ga::no );
		dualPoint = c3ga::applyUnitVersor( vertexVersor, dualPoint );
		c3ga::vectorE3GA subCubeCenter;
		subCubeCenter = dualPoint;

		c3ga::vectorE3GA quadVertices[4];
		for( int index = 0; index < 4; index++ )
		{
			double* vertex = subCubeVertex[ subCubeFace[ face ][ index ] ];
			c3ga::vectorE3GA point( c3ga::vectorE3GA::coord_e1_e2_e3, vertex[0], vertex[1], vertex[2] );
			dualPoint.set( c3ga::no + point + 0.5 * c3ga::norm2( point ) * c3ga::ni );
			dualPoint = c3ga::applyUnitVersor( vertexVersor, dualPoint );		// This should always be homogenized.
			quadVertices[ index ] = dualPoint;
		}

		glBegin( GL_QUADS );
		for( int index = 0; index < 4; index++ )
		{
			glTexCoord2dv( subCubeTextureCoordinates[ index ] );
			c3ga::vectorE3GA* vertex = &quadVertices[ index ];
			glVertex3d( vertex->get_e1(), vertex->get_e2(), vertex->get_e3() );
		}
		glEnd();

		if( mode == GL_RENDER )
		{
			if( subCube->bandaged )
			{
				glDisable( GL_TEXTURE_2D );
				glDisable( GL_LIGHTING );
				glBegin( GL_QUADS );
				glColor3d( subCube->bandageColor.get_e1(), subCube->bandageColor.get_e2(), subCube->bandageColor.get_e3() );
				c3ga::vectorE3GA center( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
				for( int index = 0; index < 4; index++ )
					center += quadVertices[ index ];
				center *= 1.0 / 4.0;
				for( int index = 0; index < 4; index++ )
				{
					c3ga::vectorE3GA vertex = center + ( quadVertices[ index ] - center ) * 0.6;
					vertex = vertex + ( center - subCubeCenter ) * 0.05;
					glVertex3d( vertex.get_e1(), vertex.get_e2(), vertex.get_e3() );
				}
				glEnd();
			}

			bool highlightFace = false;
			//c3ga::vectorE3GA highlightColor( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0 - faceColor.get_e1(), 1.0 - faceColor.get_e2(), 1.0 - faceColor.get_e3() );
			c3ga::vectorE3GA highlightColor( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 );
			if( highlightInvariants )
			{
				if( comparativeSubCube && comparativeSubCube->faceData[ face ].id == subCube->faceData[ face ].id )
				{
					if( subCube->faceData[ face ].id != -1 )
					{
						highlightFace = true;
						highlightColor.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.5, 0.5, 0.5 );
					}
				}
			}
			else if( selectedFaceId && *selectedFaceId >= 0 )
			{
				if( comparativeSubCube && comparativeSubCube->faceData[ face ].id == *selectedFaceId )
				{
					highlightFace = true;
					if( subCube->faceData[ face ].id == *selectedFaceId )
						highlightColor.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );
					else
						highlightColor.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
				}
				else if( subCube->faceData[ face ].id == *selectedFaceId )
					highlightFace = true;
			}

			if( highlightFace )
			{
				glDisable( GL_TEXTURE_2D );
				glDisable( GL_LIGHTING );
				glLineWidth( 2.f );
				glBegin( GL_LINE_LOOP );
				glColor3d( highlightColor.get_e1(), highlightColor.get_e2(), highlightColor.get_e3() );
				c3ga::vectorE3GA delta = c3ga::gp( faceNormal, 0.1 );
				for( int index = 0; index < 4; index++ )
				{
					c3ga::vectorE3GA vertex = quadVertices[ index ] + delta;
					glVertex3d( vertex.get_e1(), vertex.get_e2(), vertex.get_e3() );
				}
				glEnd();
			}
		}

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
RubiksCube::Plane RubiksCube::TranslateFace( Face face ) const
{
	Plane plane;

	switch( face )
	{
		case NEG_X:
		case POS_X:
		{
			plane.axis = X_AXIS;
			break;
		}
		case NEG_Y:
		case POS_Y:
		{
			plane.axis = Y_AXIS;
			break;
		}
		case NEG_Z:
		case POS_Z:
		{
			plane.axis = Z_AXIS;
			break;
		}
	}

	switch( face )
	{
		case NEG_X:
		case NEG_Y:
		case NEG_Z:
		{
			plane.index = 0;
			break;
		}
		case POS_X:
		case POS_Y:
		case POS_Z:
		{
			plane.index = subCubeMatrixSize - 1;
			break;
		}
	}

	return plane;
}

//==================================================================================================
/*static*/ RubiksCube::Color RubiksCube::TranslateFaceColor( Face face )
{
	switch( face )
	{
		case NEG_X: return WHITE;
		case POS_X: return YELLOW;
		case NEG_Y: return GREEN;
		case POS_Y: return BLUE;
		case NEG_Z: return ORANGE;
		case POS_Z: return RED;
	}

	return GREY;
}

//==================================================================================================
/*static*/ bool RubiksCube::TranslateGrip( Grip& grip, const Coordinates& coords, Face face )
{
	// If I'm touching the given face of the given sub-cube, what are
	// the two rotation planes that I have a grip on?
	switch( face )
	{
		case NEG_X:
		case POS_X:
		{
			grip.plane[0].axis = Y_AXIS;
			grip.plane[0].index = coords.y;
			grip.plane[1].axis = Z_AXIS;
			grip.plane[1].index = coords.z;
			return true;
		}
		case NEG_Y:
		case POS_Y:
		{
			grip.plane[0].axis = X_AXIS;
			grip.plane[0].index = coords.x;
			grip.plane[1].axis = Z_AXIS;
			grip.plane[1].index = coords.z;
			return true;
		}
		case NEG_Z:
		case POS_Z:
		{
			grip.plane[0].axis = X_AXIS;
			grip.plane[0].index = coords.x;
			grip.plane[1].axis = Y_AXIS;
			grip.plane[1].index = coords.y;
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

	if( !enforceBandaging )
		RotatePlane( rotation.plane, rotationDirection, rotationCount );
	else
	{
		RotationSequence rotationSequence;
		GenerateRotationSequenceForBandaging( rotation, rotationSequence );
		if( rotationSequence.size() == 0 )
			return false;

		enforceBandaging = false;
		for( RotationSequence::const_iterator iter = rotationSequence.cbegin(); iter != rotationSequence.cend(); iter++ )
			Apply( *iter );
		enforceBandaging = true;
	}

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

	// Lastly, fixup any bandage coordinates.
	for( int i = 0; i < subCubeMatrixSize; i++ )
	{
		for( int j = 0; j < subCubeMatrixSize; j++ )
		{
			SubCube* subCube = SubCubeIndexPlane( plane, i, j );
			if( subCube->bandaged && subCube->bandageCoords.x >= 0 )
			{
				Coordinates coords;
				switch( plane.axis )
				{
					case X_AXIS:
					{
						coords.x = subCube->bandageCoords.x;
						coords.y = subCubeMatrixSize - 1 - subCube->bandageCoords.z;
						coords.z = subCube->bandageCoords.y;
						break;
					}
					case Y_AXIS:
					{
						coords.x = subCube->bandageCoords.z;
						coords.y = subCube->bandageCoords.y;
						coords.z = subCubeMatrixSize - 1 - subCube->bandageCoords.x;
						break;
					}
					case Z_AXIS:
					{
						coords.x = subCubeMatrixSize - 1 - subCube->bandageCoords.y;
						coords.y = subCube->bandageCoords.x;
						coords.z = subCube->bandageCoords.z;
						break;
					}
				}

				if( !this->ValidMatrixCoordinates( coords ) )
				{
					int b = 0;
				}

				subCube->bandageCoords = coords;
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
const RubiksCube::SubCube* RubiksCube::SubCubeIndexPlane( Plane plane, int i, int j ) const
{
	return const_cast< RubiksCube* >( this )->SubCubeIndexPlane( plane, i, j );
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

	Swap< int >( subCube0->id, subCube1->id );
	Swap< Coordinates >( subCube0->bandageCoords, subCube1->bandageCoords );
	Swap< bool >( subCube0->bandaged, subCube1->bandaged );
	Swap< c3ga::vectorE3GA >( subCube0->bandageColor, subCube1->bandageColor );
}

//==================================================================================================
template< typename Type >
/*static*/ void RubiksCube::Swap( Type& t0, Type& t1 )
{
	Type temp = t0;
	t0 = t1;
	t1 = temp;
}

//==================================================================================================
bool RubiksCube::Select( unsigned int* hitBuffer, int hitBufferSize, int hitCount, Grip* grip /*= 0*/, int* faceId /*= 0*/ ) const
{
	bool grippingCube = false;
	if( faceId )
		*faceId = -1;

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
			Coordinates coords;
			coords.x = ( int )hitRecord[3];
			coords.y = ( int )hitRecord[4];
			coords.z = ( int )hitRecord[5];
			Face face = ( Face )hitRecord[6];

			const SubCube* subCube = Matrix( coords );
			wxASSERT( subCube );
			if( subCube->faceData[ face ].color != GREY )
			{
				float minZ = float( hitRecord[1] ) / float( 0x7FFFFFFF );
				if( minZ < smallestZ )
				{
					smallestZ = minZ;
					if( grip )
						grippingCube = TranslateGrip( *grip, coords, face );
					if( faceId )
						*faceId = subCube->faceData[ face ].id;
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

		RotationSequence::iterator iter0 = rotationSequence.begin();
		while( iter0 != rotationSequence.end() )
		{
			Rotation rotation0 = *iter0;
			if( fabs( rotation0.angle ) < epsilon )
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

			while( iter1 != rotationSequence.end() )
			{
				Rotation rotation1 = *iter1;

				if( rotation0.plane.axis != rotation1.plane.axis )
					break;
				else
				{
					if( rotation0.plane.index == rotation1.plane.index )
					{
						if( fabs( rotation0.angle + rotation1.angle ) < epsilon )
						{
							rotationSequence.erase( iter0 );
							rotationSequence.erase( iter1 );
							optimizationFound = true;
							break;
						}

						rotation0.angle += rotation1.angle;
						rotationSequence.erase( iter1 );
						*iter0 = rotation0;
						optimizationFound = true;
						break;
					}
				
					iter1++;
				}
			}

			if( !optimizationFound )
				iter0++;
			else
				break;
		}
	}
	while( optimizationFound );
}

//==================================================================================================
/*static*/ void RubiksCube::InvertRelativeRotation( RelativeRotation& relativeRotation )
{
	switch( relativeRotation.type )
	{
		case RelativeRotation::L:	relativeRotation.type = RelativeRotation::Li;	return;
		case RelativeRotation::R:	relativeRotation.type = RelativeRotation::Ri;	return;
		case RelativeRotation::D:	relativeRotation.type = RelativeRotation::Di;	return;
		case RelativeRotation::U:	relativeRotation.type = RelativeRotation::Ui;	return;
		case RelativeRotation::B:	relativeRotation.type = RelativeRotation::Bi;	return;
		case RelativeRotation::F:	relativeRotation.type = RelativeRotation::Fi;	return;
		case RelativeRotation::Li:	relativeRotation.type = RelativeRotation::L;	return;
		case RelativeRotation::Ri:	relativeRotation.type = RelativeRotation::R;	return;
		case RelativeRotation::Di:	relativeRotation.type = RelativeRotation::D;	return;
		case RelativeRotation::Ui:	relativeRotation.type = RelativeRotation::U;	return;
		case RelativeRotation::Bi:	relativeRotation.type = RelativeRotation::B;	return;
		case RelativeRotation::Fi:	relativeRotation.type = RelativeRotation::F;	return;
	}

	wxASSERT( false );
}

//==================================================================================================
/*static*/ void RubiksCube::InvertRelativeRotationSequence( RelativeRotationSequence& relativeRotationSequence )
{
	if( relativeRotationSequence.size() <= 0 )
		return;

	RelativeRotationSequence::iterator iter = relativeRotationSequence.begin();
	RelativeRotation relativeRotation = *iter;

	relativeRotationSequence.erase( iter );
	InvertRelativeRotationSequence( relativeRotationSequence );

	InvertRelativeRotation( relativeRotation );

	relativeRotationSequence.push_back( relativeRotation );
}

//==================================================================================================
/*static*/ void RubiksCube::InvertRotation( Rotation& rotation )
{
	rotation.angle = -rotation.angle;
}

//==================================================================================================
/*static*/ void RubiksCube::InvertRotationSequence( RotationSequence& rotationSequence )
{
	if( rotationSequence.size() <= 0 )
		return;

	RotationSequence::iterator iter = rotationSequence.begin();
	Rotation rotation = *iter;

	rotationSequence.erase( iter );
	InvertRotationSequence( rotationSequence );

	InvertRotation( rotation );

	rotationSequence.push_back( rotation );
}

//==================================================================================================
/*static*/ void RubiksCube::ConcatinateRotationSequence( RotationSequence& rotationSequence, const RotationSequence& otherRotationSequence )
{
	RotationSequence::const_iterator iter = otherRotationSequence.begin();
	while( iter != otherRotationSequence.end() )
	{
		rotationSequence.push_back( *iter );
		iter++;
	}
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

#ifdef SOLVER_DEBUG
	c3ga::bivectorE3GA blade = c3ga::op( perspective.rAxis, perspective.uAxis );
	c3ga::trivectorE3GA I( c3ga::trivectorE3GA::coord_e1e2e3, -1.0 );
	c3ga::vectorE3GA vec = c3ga::lc( blade, I );
	double length = c3ga::norm( vec - perspective.fAxis );
	wxASSERT( length < 1e-6 );
#endif //SOLVER_DEBUG

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

	perspective.PerspectiveSpaceToWorldSpace( axis, axis );

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

		int processCount = 1;
		if( token.c_str()[0] == '2' )
		{
			processCount = 2;
			const char* newToken = &token.c_str()[1];
			token = newToken;
		}

		for( int count = 0; count < processCount; count++ )
		{
			RubiksCube::RelativeRotation relativeRotation;
			if( TranslateRelativeRotation( token, relativeRotation ) )
				relativeRotationSequence.push_back( relativeRotation );
			else
				return false;
		}
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
	if( token == "L" || token == "l" )
		relativeRotation.type = RubiksCube::RelativeRotation::L;
	else if( token == "R" || token == "r" )
		relativeRotation.type = RubiksCube::RelativeRotation::R;
	else if( token == "D" || token == "d" )
		relativeRotation.type = RubiksCube::RelativeRotation::D;
	else if( token == "U" || token == "u" )
		relativeRotation.type = RubiksCube::RelativeRotation::U;
	else if( token == "B" || token == "b" )
		relativeRotation.type = RubiksCube::RelativeRotation::B;
	else if( token == "F" || token == "f" )
		relativeRotation.type = RubiksCube::RelativeRotation::F;
	else if( token == "Li" || token == "li" || token == "L'" || token == "l'" )
		relativeRotation.type = RubiksCube::RelativeRotation::Li;
	else if( token == "Ri" || token == "ri" || token == "R'" || token == "r'" )
		relativeRotation.type = RubiksCube::RelativeRotation::Ri;
	else if( token == "Di" || token == "di" || token == "D'" || token == "d'" )
		relativeRotation.type = RubiksCube::RelativeRotation::Di;
	else if( token == "Ui" || token == "ui" || token == "U'" || token == "u'" )
		relativeRotation.type = RubiksCube::RelativeRotation::Ui;
	else if( token == "Bi" || token == "bi" || token == "B'" || token == "b'" )
		relativeRotation.type = RubiksCube::RelativeRotation::Bi;
	else if( token == "Fi" || token == "fi" || token == "F'" || token == "f'" )
		relativeRotation.type = RubiksCube::RelativeRotation::Fi;
	else
		return false;

	// Let the lower-case versions move the nearest inner layer to the outer layer.
	if( ::islower( token.c_str()[0] ) )
		relativeRotation.planeIndex = 1;

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
const RubiksCube::SubCube* RubiksCube::Matrix( const Coordinates& actualCoords ) const
{
	if( !ValidMatrixCoordinates( actualCoords ) )
		return 0;

	return &subCubeMatrix[ actualCoords.x ][ actualCoords.y ][ actualCoords.z ];
}

//==================================================================================================
RubiksCube::SubCube* RubiksCube::Matrix( const Coordinates& actualCoords )
{
	if( !ValidMatrixCoordinates( actualCoords ) )
		return 0;

	return &subCubeMatrix[ actualCoords.x ][ actualCoords.y ][ actualCoords.z ];
}

//==================================================================================================
const RubiksCube::SubCube* RubiksCube::Matrix( const Coordinates& relativeCoords, const Perspective& perspective ) const
{
	Coordinates actualCoords;
	RelativeToActual( relativeCoords, actualCoords, perspective );
	return Matrix( actualCoords );
}

//==================================================================================================
void RubiksCube::Perspective::PerspectiveSpaceToWorldSpace( const c3ga::vectorE3GA& perspectiveVector, c3ga::vectorE3GA& worldVector ) const
{
	worldVector =
			rAxis * perspectiveVector.get_e1() +
			uAxis * perspectiveVector.get_e2() +
			fAxis * perspectiveVector.get_e3();
}

//==================================================================================================
void RubiksCube::Perspective::PerspectiveSpaceFromWorldSpace( const c3ga::vectorE3GA& worldVector, c3ga::vectorE3GA& perspectiveVector ) const
{
	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, rAxis.get_e1(), uAxis.get_e1(), fAxis.get_e1() );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, rAxis.get_e2(), uAxis.get_e2(), fAxis.get_e2() );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, rAxis.get_e3(), uAxis.get_e3(), fAxis.get_e3() );
	perspectiveVector =
			xAxis * worldVector.get_e1() +
			yAxis * worldVector.get_e2() +
			zAxis * worldVector.get_e3();
}

//==================================================================================================
int RubiksCube::PreCoordinateTransform( int coordinate ) const
{
	// This is done so that we can detect a sign change in a zero index.
	if( coordinate == 0 )
		coordinate += subCubeMatrixSize;
	return coordinate;
}

//==================================================================================================
int RubiksCube::PostCoordinateTransform( int coordinate ) const
{
	// Map the index in the other direction.
	if( coordinate < 0 )
		coordinate = subCubeMatrixSize - 1 - ( -coordinate ) % subCubeMatrixSize;
	coordinate %= subCubeMatrixSize;
	return coordinate;
}

//==================================================================================================
void RubiksCube::RelativeToActual( const Coordinates& relativeCoords, Coordinates& actualCoords, const Perspective& perspective ) const
{
	c3ga::vectorE3GA perspectiveVector( c3ga::vectorE3GA::coord_e1_e2_e3,
												PreCoordinateTransform( relativeCoords.x ),
												PreCoordinateTransform( relativeCoords.y ),
												PreCoordinateTransform( relativeCoords.z ) );
	c3ga::vectorE3GA worldVector;
	perspective.PerspectiveSpaceToWorldSpace( perspectiveVector, worldVector );
	actualCoords.x = PostCoordinateTransform( int( worldVector.get_e1() ) );
	actualCoords.y = PostCoordinateTransform( int( worldVector.get_e2() ) );
	actualCoords.z = PostCoordinateTransform( int( worldVector.get_e3() ) );
}

//==================================================================================================
void RubiksCube::RelativeFromActual( const Coordinates& actualCoords, Coordinates& relativeCoords, const Perspective& perspective ) const
{
	c3ga::vectorE3GA worldVector( c3ga::vectorE3GA::coord_e1_e2_e3,
												PreCoordinateTransform( actualCoords.x ),
												PreCoordinateTransform( actualCoords.y ),
												PreCoordinateTransform( actualCoords.z ) );
	c3ga::vectorE3GA perspectiveVector;
	perspective.PerspectiveSpaceFromWorldSpace( worldVector, perspectiveVector );
	relativeCoords.x = PostCoordinateTransform( int( perspectiveVector.get_e1() ) );
	relativeCoords.y = PostCoordinateTransform( int( perspectiveVector.get_e2() ) );
	relativeCoords.z = PostCoordinateTransform( int( perspectiveVector.get_e3() ) );
}

//==================================================================================================
bool RubiksCube::ValidMatrixCoordinates( const Coordinates& coords ) const
{
	if( coords.x < 0 || coords.y < 0 || coords.z < 0 )
		return false;
	if( coords.x > subCubeMatrixSize - 1 )
		return false;
	if( coords.y > subCubeMatrixSize - 1 )
		return false;
	if( coords.z > subCubeMatrixSize - 1 )
		return false;
	return true;
}

//==================================================================================================
void RubiksCube::Rotation::Invert( void )
{
	angle *= -1.0;
}

//==================================================================================================
void RubiksCube::Rotation::GetInverse( Rotation& rotation ) const
{
	rotation.plane = plane;
	rotation.angle = -angle;
}

//==================================================================================================
void RubiksCube::Rotation::SetInverse( const Rotation& rotation )
{
	plane = rotation.plane;
	angle = -rotation.angle;
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
	
	// TODO: We need to save bandaging info.

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

	// TODO: We need to load bandaging info.

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

		Coordinates coords;
		coords.x = int( xLong );
		coords.y = int( yLong );
		coords.z = int( zLong );
		SubCube* subCube = Matrix( coords );
		if( !subCube )
			return false;

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
		rotation.plane.index = RandomNumber( 0, subCubeMatrixSize - 1 );
		rotation.plane.axis = Axis( RandomNumber( 0, 2 ) );
		rotation.angle = double( RandomNumber( 1, 3 ) ) * M_PI / 2.0;
		rotationSequence->push_back( rotation );
	}

	if( apply )
		ApplySequence( *rotationSequence );
}

//==================================================================================================
/*static*/ int RubiksCube::RandomNumber( int min, int max )
{
#if 0
	int randomNumber = int( double( min ) + double( rand() ) / double( RAND_MAX ) * double( max - min ) );
	if( randomNumber < min )
		randomNumber = min;
	else if( randomNumber > max )
		randomNumber = max;
	return randomNumber;
#else
	return min + rand() % ( max - min + 1 );
#endif
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

//==================================================================================================
/*static*/ bool RubiksCube::AreOppositeFaces( Face face0, Face face1 )
{
	if( ( face0 == NEG_X && face1 == POS_X ) || ( face0 == POS_X && face1 == NEG_X ) )
		return true;
	if( ( face0 == NEG_Y && face1 == POS_Y ) || ( face0 == POS_Y && face1 == NEG_Y ) )
		return true;
	if( ( face0 == NEG_Z && face1 == POS_Z ) || ( face0 == POS_Z && face1 == NEG_Z ) )
		return true;
	return false;
}

//==================================================================================================
/*static*/ bool RubiksCube::AreAdjacentFaces( Face face0, Face face1 )
{
	if( AreOppositeFaces( face0, face1 ) )
		return false;
	if( face0 == face1 )
		return false;
	return true;
}

//==================================================================================================
// Here we do a linear search which would be very impractical if we decided to work
// with cubes of high order.  We could use a map to speed this up.  We would just
// need to remember to update the map for each rotation.  For now, I'm keeping it simple.
const RubiksCube::SubCube* RubiksCube::FindSubCubeById( int subCubeId ) const
{
	// Another sillyness to consider here, of course, is that we're searching
	// the entire cube: the inner, non-exposed cubes, as well as the exposed cubes.
	// We only need to track and search the exposed cubes.  Whatever.
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				const SubCube* subCube = &subCubeMatrix[x][y][z];
				if( subCube->id == subCubeId )
					return subCube;
			}
		}
	}

	return 0;
}

//==================================================================================================
RubiksCube::SubCube* RubiksCube::FindSubCubeByFaceId( int faceId )
{
	for( int x = 0; x < subCubeMatrixSize; x++ )
	{
		for( int y = 0; y < subCubeMatrixSize; y++ )
		{
			for( int z = 0; z < subCubeMatrixSize; z++ )
			{
				SubCube* subCube = &subCubeMatrix[x][y][z];
				for( int face = 0; face < CUBE_FACE_COUNT; face++ )
					if( subCube->faceData[ face ].id == faceId )
						return subCube;
			}
		}
	}

	return nullptr;
}

//==================================================================================================
const RubiksCube::SubCube* RubiksCube::FindSubCubeByFaceId( int faceId ) const
{
	return const_cast< RubiksCube* >( this )->FindSubCubeByFaceId( faceId );
}

//==================================================================================================
/*static*/ int RubiksCube::PlaneContainingSubCube( Axis axis, const SubCube* subCube )
{
	switch( axis )
	{
		case X_AXIS: return subCube->coords.x;
		case Y_AXIS: return subCube->coords.y;
		case Z_AXIS: return subCube->coords.z;
	}

	return -1;
}

// RubiksCube.cpp
