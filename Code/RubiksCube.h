// RubiksCube.h

//==================================================================================================
class RubiksCube
{
public:

	RubiksCube( int subCubeMatrixSize = 3, int colorCount = 6 );
	~RubiksCube( void );

	enum RelativeRotation
	{
		L,
		R,
		D,
		U,
		B,
		F,
		Li,
		Ri,
		Di,
		Ui,
		Bi,
		Fi,
	};

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
		YELLOW,		// +X
		BLUE,		// +Y
		RED,		// +Z
		WHITE,		// -X
		GREEN,		// -Y
		ORANGE,		// -Z
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

	struct Perspective
	{
		// This should always be a right-handed orthonormal system.
		// Not only this, but each axes vector should consist of two
		// zeros with the remaining component being +/-1.
		c3ga::vectorE3GA rAxis;
		c3ga::vectorE3GA uAxis;
		c3ga::vectorE3GA fAxis;
	};

	typedef std::list< Rotation > RotationSequence;
	typedef std::list< RelativeRotation > RelativeRotationSequence;

	void Render( GLenum mode, const Rotation& rotation, const Size& size ) const;
	bool Apply( const Rotation& rotation, Rotation* invariantRotation = 0, Rotation* reverseRotation = 0 );
	bool ApplySequence( const RotationSequence& rotationSequence );
	bool Select( unsigned int* hitBuffer, int hitBufferSize, int hitCount, Grip& grip ) const;
	bool IsInSolvedState( void ) const;

	int SubCubeMatrixSize( void ) const;

	static c3ga::vectorE3GA TranslateColor( Color color );
	static c3ga::vectorE3GA TranslateAxis( Axis axis );
	static c3ga::vectorE3GA TranslateNormal( Face face );
	static bool TranslateGrip( Grip& grip, int x, int y, int z, Face face );
	static void TranslateAxis( wxString& axisString, Axis axis );
	bool TranslateRotation( const Perspective& perspective, RelativeRotation relativeRotation, Rotation& rotation ) const;
	bool TranslateRotationSequence( const Perspective& perspective, const RelativeRotationSequence& relativeRotationSequence, RotationSequence& rotationSequence ) const;

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