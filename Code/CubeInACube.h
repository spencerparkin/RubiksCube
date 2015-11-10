// CubeInACube.h

//==================================================================================================
class CubeInACube : public Solver
{
public:

	CubeInACube( void );
	virtual ~CubeInACube( void );

	virtual bool GetReady( void ) override;

	virtual bool MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence ) override;

	std::string CompoundRotate( const std::string& rotationString, int min, int max );

	void RotateEdgesOfCorner( const RubiksCube* rubiksCube, const RubiksCube::Perspective& cornerPerspectiveA, const RubiksCube::Perspective& cornerPerspectiveB, RubiksCube::RotationSequence& rotationSequence );

	int layerCount;
};

// CubeInACube.h