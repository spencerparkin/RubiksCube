// RubiksCube.h

//==================================================================================================
class RubiksCube
{
public:

	RubiksCube( int subCubeMatrixSize = 3, int colorCount = 6 );
	~RubiksCube( void );

	enum Axis
	{
		X_AXIS,
		Y_AXIS,
		Z_AXIS,
	};

	enum Face
	{
		NEG_X,
		POS_X,
		NEG_Y,
		POS_Y,
		NEG_Z,
		POS_Z,
		CUBE_FACE_COUNT,
	};

	enum Color
	{
		GREY,
		RED,
		GREEN,
		BLUE,
		YELLOW,
		MEGENTA,
		CYAN,
		WHITE,
	};

	enum RotationDirection
	{
		ROTATE_CCW,
		ROTATE_CW,
	};

	struct Plane
	{
		Axis axis;
		int index;
	};

	struct Rotation
	{
		Plane plane;
		double angle;
	};

	struct Grip
	{
		Plane plane[2];
	};

	struct Size
	{
		double cubeWidthHeightAndDepth;
		double subCubeWidthHeightAndDepth;
	};

	void Render( GLenum mode, const Rotation& rotation, const Size& size ) const;
	bool Apply( const Rotation& rotation, Rotation* invariantRotation = 0, Rotation* reverseRotation = 0 );
	bool Select( unsigned int* hitBuffer, int hitBufferSize, int hitCount, Grip& grip ) const;

	int SubCubeMatrixSize( void ) const;

	static c3ga::vectorE3GA TranslateColor( Color color );
	static c3ga::vectorE3GA TranslateAxis( Axis axis );
	static c3ga::vectorE3GA TranslateNormal( Face face );
	static bool TranslateGrip( Grip& grip, int x, int y, int z, Face face );
	static void TranslateAxis( wxString& axisString, Axis axis );

private:

	void RotatePlane( Plane plane, RotationDirection rotationDirection, int rotationCount );
	void RotatePlaneCCW( Plane plane );

	struct SubCube
	{
		Color faceColor[ CUBE_FACE_COUNT ];
	};

	void RenderSubCube( GLenum mode, const SubCube* subCube, const c3ga::evenVersor& vertexVersor, const c3ga::evenVersor& normalVersor ) const;

	SubCube* SubCubeIndexPlane( Plane plane, int i, int j );
	static void SwapColors( SubCube* subCube, int i, int j );
	static void SwapSubCubes( SubCube* subCube0, SubCube* subCube1 );

	SubCube*** subCubeMatrix;
	int subCubeMatrixSize;

	static double subCubeVertex[8][3];
	static int subCubeFace[ CUBE_FACE_COUNT ][4];
};

// RubiksCube.h