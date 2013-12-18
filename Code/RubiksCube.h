// RubiksCube.h

//==================================================================================================
class RubiksCube
{
public:

	RubiksCube( int subCubeMatrixSize = 3, bool loadTextures = true );
	~RubiksCube( void );

	struct RelativeRotation
	{
		enum Type
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

		RelativeRotation( Type type = U, int planeIndex = 0 );

		Type type;
		int planeIndex;
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
		// Color used for internal faces:
		GREY,

		// Colors used for external faces:
		YELLOW,		// +X
		BLUE,		// +Y
		RED,		// +Z
		WHITE,		// -X
		GREEN,		// -Y
		ORANGE,		// -Z

		MAX_COLORS,
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

	struct SubCube
	{
		struct FaceData
		{
			Color color;
			int id;
		};

		FaceData faceData[ CUBE_FACE_COUNT ];
		int x, y, z;
	};

	typedef void ( *CopyMapFunc )( int& x, int& y, int& z );
	bool Copy( const RubiksCube& rubiksCube, CopyMapFunc copyMapFunc );
	static void CopyMap( int& x, int& y, int& z );

	const SubCube* Matrix( int x, int y, int z ) const;
	bool ValidMatrixCoordinates( int x, int y, int z ) const;

	typedef std::vector< const SubCube* > SubCubeVector;

	void CollectSubCubes( Color* colorArray, int colorCount, SubCubeVector& subCubeVector ) const;
	const SubCube* CollectSubCube( Color* colorArray, int colorCount ) const;

	static bool CubeHasColor( const SubCube* subCube, Color color );
	static bool CubeHasAllColors( const SubCube* subCube, Color* colorArray, int colorCount );

	static int ExposedColors( const SubCube* subCube, Color* colorArray );
	static int ExposedFaces( const SubCube* subCube, Face* faceArray );

	typedef std::list< Rotation > RotationSequence;
	typedef std::list< RelativeRotation > RelativeRotationSequence;

	void Render( GLenum mode, const Rotation& rotation, const Size& size,
										int* selectedFaceId = 0,
										const RubiksCube* comparativeRubiksCube = 0,
										bool highlightInvariants = false ) const;

	bool Apply( const Rotation& rotation, Rotation* invariantRotation = 0, Rotation* reverseRotation = 0 );
	bool ApplySequence( const RotationSequence& rotationSequence );
	bool Select( unsigned int* hitBuffer, int hitBufferSize, int hitCount, Grip* grip = 0, int* faceId = 0 ) const;
	bool IsInSolvedState( void ) const;

	int SubCubeMatrixSize( void ) const;

	static c3ga::vectorE3GA TranslateColor( Color color );
	static c3ga::vectorE3GA TranslateAxis( Axis axis );
	static c3ga::vectorE3GA TranslateNormal( Face face );
	static Face TranslateNormal( const c3ga::vectorE3GA& unitNormal );
	static bool TranslateGrip( Grip& grip, int x, int y, int z, Face face );
	static void TranslateAxis( wxString& axisString, Axis axis );
	Plane TranslateFace( Face face ) const;
	bool TranslateRotation( const Perspective& perspective, const RelativeRotation& relativeRotation, Rotation& rotation ) const;
	bool TranslateRotationSequence( const Perspective& perspective, const RelativeRotationSequence& relativeRotationSequence, RotationSequence& rotationSequence ) const;
	bool TranslateRelativeRotation( const std::string& relativeRotationString, RelativeRotation& relativeRotation ) const;
	bool ParseRelativeRotationSequenceString( const std::string& relativeRotationSequenceString, RelativeRotationSequence& relativeRotationSequence ) const;
	static void CompressRotationSequence( RotationSequence& rotationSequence );

	bool SaveToFile( const wxString& file ) const;
	
	static bool SaveToFile( const RubiksCube* rubiksCube, const wxString& file );
	static RubiksCube* LoadFromFile( const wxString& file );

	void Scramble( int seed, int rotationCount, RotationSequence* rotationSequence = 0, bool apply = true );
	Solver* MakeSolver( void ) const;

	static int RandomNumber( int min, int max );

	static bool AreOppositeFaces( Face face0, Face face1 );
	static bool AreAdjacentFaces( Face face0, Face face1 );

private:

	bool SaveToXml( wxXmlNode* xmlNode ) const;
	bool LoadFromXml( const wxXmlNode* xmlNode );

	static bool SaveIntegerToXml( wxXmlNode* xmlNode, const wxString& name, int integer );
	static bool LoadIntegerFromXml( const wxXmlNode* xmlNode, const wxString& name, int& integer );

	void RotatePlane( Plane plane, RotationDirection rotationDirection, int rotationCount );
	void RotatePlaneCCW( Plane plane );

	void RenderSubCube( GLenum mode, const SubCube* subCube,
								const c3ga::evenVersor& vertexVersor,
								const c3ga::evenVersor& normalVersor,
								int* selectedFaceId,
								const SubCube* comparativeSubCube,
								bool highlightInvariants ) const;

	SubCube* SubCubeIndexPlane( Plane plane, int i, int j );
	static void SwapFaces( SubCube* subCube, int i, int j );
	static void SwapFaces( SubCube::FaceData& faceData0, SubCube::FaceData& faceData1 );
	static void SwapSubCubes( SubCube* subCube0, SubCube* subCube1 );

	SubCube*** subCubeMatrix;
	int subCubeMatrixSize;

	static double subCubeVertex[8][3];
	static int subCubeFace[ CUBE_FACE_COUNT ][4];
	static double subCubeTextureCoordinates[4][2];

	GLuint textures[ MAX_COLORS ];
	static const char* textureFiles[ MAX_COLORS ];

	void LoadTextures( void );
	void UnloadTextures( void );
};

// RubiksCube.h