// RubiksCube.h

//==================================================================================================
class RubiksCube
{
public:

	RubiksCube( int subCubeMatrixSize = 3 );
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

		void Invert( void );
		void GetInverse( Rotation& rotation ) const;
		void SetInverse( const Rotation& rotation );
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

		void PerspectiveSpaceToWorldSpace( const c3ga::vectorE3GA& perspectiveVector, c3ga::vectorE3GA& worldVector ) const;
		void PerspectiveSpaceFromWorldSpace( const c3ga::vectorE3GA& worldVector, c3ga::vectorE3GA& perspectiveVector ) const;
	};

	struct Coordinates
	{
		Coordinates( int x = -1, int y = -1, int z = -1 )
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		int x, y, z;
	};

	struct SubCube
	{
		struct TexCoords
		{
			float data[4][2];
		};

		struct FaceData
		{
			Color color;
			int id;
			TexCoords texCoords;
		};

		FaceData faceData[ CUBE_FACE_COUNT ];
		Coordinates coords;
		mutable Coordinates bandageCoords;
		c3ga::vectorE3GA bandageColor;
		bool bandaged;
		int id;
	};

	void CalcCubieTexCoords( SubCube::TexCoords& texCoords, Face face, const Coordinates& coords );

	typedef void ( *CopyMapFunc )( Coordinates& );
	bool Copy( const RubiksCube& rubiksCube, CopyMapFunc copyMapFunc );
	static void CopyMap( Coordinates& coords );

	SubCube* Matrix( const Coordinates& actualCoords );
	const SubCube* Matrix( const Coordinates& actualCoords ) const;
	const SubCube* Matrix( const Coordinates& relativeCoords, const Perspective& perspective ) const;
	bool ValidMatrixCoordinates( const Coordinates& actualCoords ) const;
	void RelativeToActual( const Coordinates& relativeCoords, Coordinates& actualCoords, const Perspective& perspective ) const;
	void RelativeFromActual( const Coordinates& actualCoords, Coordinates& relativeCoords, const Perspective& perspective ) const;
	int PreCoordinateTransform( int coordinate ) const;
	int PostCoordinateTransform( int coordinate ) const;

	typedef std::vector< const SubCube* > SubCubeVector;

	void CollectSubCubes( Color* colorArray, int colorCount, SubCubeVector& subCubeVector ) const;
	const SubCube* CollectSubCube( Color* colorArray, int colorCount ) const;

	static bool CubeHasColor( const SubCube* subCube, Color color );
	static bool CubeHasAllColors( const SubCube* subCube, Color* colorArray, int colorCount );

	static int ExposedColors( const SubCube* subCube, Color* colorArray );
	static int ExposedFaces( const SubCube* subCube, Face* faceArray );

	SubCube* SubCubeBandageRepresentative( SubCube* subCube );
	const SubCube* SubCubeBandageRepresentative( const SubCube* subCube ) const;

	void CollectBandagedCubies( SubCube* givenSubCube, SubCubeVector& subCubeVector );
	void CollectCubiesInPlaneOfRotation( const Rotation& rotation, SubCubeVector& subCubeVector ) const;

	typedef std::list< Rotation > RotationSequence;
	typedef std::list< RelativeRotation > RelativeRotationSequence;

	void Render( GLenum mode, const Rotation& rotation, const Size& size,
										int* selectedFaceId = 0,
										const RubiksCube* comparativeRubiksCube = 0,
										bool highlightInvariants = false ) const;
	void Render( GLenum mode, const RotationSequence& rotationSequence, const Size& size,
										int* selectedFaceId = 0,
										const RubiksCube* comparativeRubiksCube = 0,
										bool highlightInvariants = false ) const;

	bool Apply( const Rotation& rotation, Rotation* invariantRotation = 0, Rotation* reverseRotation = 0 );
	bool ApplySequence( const RotationSequence& rotationSequence );
	bool Select( unsigned int* hitBuffer, int hitBufferSize, int hitCount, Grip* grip = 0, int* faceId = 0 ) const;
	bool IsInSolvedState( void ) const;

	bool GenerateRotationSequenceForBandaging( const Rotation& rotation, RotationSequence& rotationSequence ) const;

	int SubCubeMatrixSize( void ) const;

	static c3ga::vectorE3GA TranslateColor( Color color );
	static c3ga::vectorE3GA TranslateAxis( Axis axis );
	static c3ga::vectorE3GA TranslateNormal( Face face );
	static Face TranslateNormal( const c3ga::vectorE3GA& unitNormal );
	static bool TranslateGrip( Grip& grip, const Coordinates& coords, Face face );
	static void TranslateAxis( wxString& axisString, Axis axis );
	static Color TranslateFaceColor( Face face );
	Plane TranslateFace( Face face ) const;
	bool TranslateRotation( const Perspective& perspective, const RelativeRotation& relativeRotation, Rotation& rotation ) const;
	bool TranslateRotationSequence( const Perspective& perspective, const RelativeRotationSequence& relativeRotationSequence, RotationSequence& rotationSequence ) const;
	bool TranslateRelativeRotation( const std::string& relativeRotationString, RelativeRotation& relativeRotation ) const;
	bool ParseRelativeRotationSequenceString( const std::string& relativeRotationSequenceString, RelativeRotationSequence& relativeRotationSequence ) const;
	static void CompressRotationSequence( RotationSequence& rotationSequence );
	static void InvertRelativeRotation( RelativeRotation& relativeRotation );
	static void InvertRelativeRotationSequence( RelativeRotationSequence& relativeRotationSequence );
	static void InvertRotation( Rotation& rotation );
	static void InvertRotationSequence( RotationSequence& rotationSequence );
	static void ConcatinateRotationSequence( RotationSequence& rotationSequence, const RotationSequence& otherRotationSequence );

	bool SaveToFile( const wxString& file ) const;
	
	static bool SaveToFile( const RubiksCube* rubiksCube, const wxString& file );
	static RubiksCube* LoadFromFile( const wxString& file );

	void Scramble( int seed, int rotationCount, RotationSequence* rotationSequence = 0, bool apply = true );
	Solver* MakeSolver( void ) const;

	static int RandomNumber( int min, int max );

	static bool AreOppositeFaces( Face face0, Face face1 );
	static bool AreAdjacentFaces( Face face0, Face face1 );

	const SubCube* FindSubCubeById( int subCubeId ) const;
	const SubCube* FindSubCubeByFaceId( int faceId, const SubCube::FaceData** faceData = nullptr ) const;
	SubCube* FindSubCubeByFaceId( int faceId, SubCube::FaceData** faceData = nullptr );

	static int PlaneContainingSubCube( Axis axis, const SubCube* subCube );

	bool EnforceBandaging( void ) const { return enforceBandaging; }
	void EnforceBandaging( bool enforce ) const { enforceBandaging = enforce; }

	void ClearBandaging( void );
	void BandageCubies( SubCube* subCubeA, SubCube* subCubeB );

	enum TextureApplication { TEX_APPLY_ENTIRE_FACE, TEX_APPLY_CUBIE_FACES };
	TextureApplication texApp;

	void ReplaceFaceTextureWithImage( Color color, wxImage* image ) const;

private:

	bool SaveToXml( wxXmlNode* xmlNode ) const;
	bool LoadFromXml( const wxXmlNode* xmlNode );

	// I'm surprised I couldn't find this function in the API docs.
	const wxXmlNode* FindNodeByName( const wxXmlNode* xmlRoot, const wxString& name, bool recursive = false );

	static bool SaveIntegerToXml( wxXmlNode* xmlNode, const wxString& name, int integer );
	static bool LoadIntegerFromXml( const wxXmlNode* xmlNode, const wxString& name, int& integer );

	void RotatePlane( Plane plane, RotationDirection rotationDirection, int rotationCount );
	void RotatePlaneCCW( Plane plane );

	void RenderSubCube( GLenum mode, int x, int y, int z,
								const RotationSequence& rotationSequence, const Size& size,
								int* selectedFaceId,
								const RubiksCube* comparativeRubiksCube,
								bool highlightInvariants ) const;
	void RenderSubCube( GLenum mode, const SubCube* subCube,
								const c3ga::evenVersor& vertexVersor,
								const c3ga::evenVersor& normalVersor,
								int* selectedFaceId,
								const SubCube* comparativeSubCube,
								bool highlightInvariants ) const;

	SubCube* SubCubeIndexPlane( Plane plane, int i, int j );
	const SubCube* SubCubeIndexPlane( Plane plane, int i, int j ) const;
	static void SwapFaces( SubCube* subCube, int i, int j );
	static void SwapFaces( SubCube::FaceData& faceData0, SubCube::FaceData& faceData1 );
	static void SwapSubCubes( SubCube* subCube0, SubCube* subCube1 );
	template< typename Type > static void Swap( Type& t0, Type& t1 );

	SubCube*** subCubeMatrix;
	int subCubeMatrixSize;
	mutable bool enforceBandaging;

	static double subCubeVertex[8][3];
	static int subCubeFace[ CUBE_FACE_COUNT ][4];
	static double subCubeTextureCoordinates[4][2];

	struct Texture
	{
		GLuint name;
		wxImage* image;
	};

	mutable Texture textures[ MAX_COLORS ];

	static const char* defaultTextureFiles[ MAX_COLORS ];

	GLuint GetTextureObject( Color color ) const;
	void FreeTextureData( void );

	enum TexCoordRotateDir { ROT_TEX_COORDS_CW, ROT_TEX_COORDS_CCW };
	void RotateTextureCoordinates( SubCube::TexCoords& texCoords, TexCoordRotateDir rotDir );
};

// RubiksCube.h