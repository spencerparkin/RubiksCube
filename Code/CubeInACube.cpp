// CubeInACube.cpp

#include "Header.h"

//==================================================================================================
CubeInACube::CubeInACube( void )
{
	layerCount = 0;
}

//==================================================================================================
/*virtual*/ CubeInACube::~CubeInACube( void )
{
}

//==================================================================================================
/*virtual*/ bool CubeInACube::GetReady( void )
{
	layerCount = 1;
	return true;
}

//==================================================================================================
std::string CubeInACube::CompoundRotate( const std::string& rotationString, int min, int max )
{
	std::string rotationSequenceString;

	for( int i = min; i <= max; i++ )
	{
		rotationSequenceString += rotationString + ":" + std::to_string(i);
		if( i + 1 <= max )
			rotationSequenceString += ", ";
	}

	return rotationSequenceString;
}

//==================================================================================================
// TODO: This doesn't quit work for cubes of odd order.
/*virtual*/ bool CubeInACube::MakeRotationSequence( const RubiksCube* rubiksCube, RubiksCube::RotationSequence& rotationSequence )
{
	c3ga::vectorE3GA xAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	c3ga::vectorE3GA yAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	c3ga::vectorE3GA zAxis( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );

	if( layerCount < rubiksCube->SubCubeMatrixSize() / 2 )
	{
		RubiksCube::Perspective cornerPerspectiveA, cornerPerspectiveB;
	
		cornerPerspectiveA.rAxis = xAxis;
		cornerPerspectiveA.uAxis = yAxis;
		cornerPerspectiveA.fAxis = zAxis;

		cornerPerspectiveB.rAxis = zAxis;
		cornerPerspectiveB.uAxis = yAxis;
		cornerPerspectiveB.fAxis = -xAxis;

		RubiksCube::RotationSequence subRotationSequence;
		RotateEdgesOfCorner( rubiksCube, cornerPerspectiveA, cornerPerspectiveB, subRotationSequence );

		RubiksCube::ConcatinateRotationSequence( rotationSequence, subRotationSequence );

		cornerPerspectiveA.rAxis = -yAxis;
		cornerPerspectiveA.uAxis = -xAxis;
		cornerPerspectiveA.fAxis = -zAxis;

		cornerPerspectiveB.rAxis = -zAxis;
		cornerPerspectiveB.uAxis = -xAxis;
		cornerPerspectiveB.fAxis = yAxis;

		subRotationSequence.clear();
		RotateEdgesOfCorner( rubiksCube, cornerPerspectiveA, cornerPerspectiveB, subRotationSequence );

		RubiksCube::InvertRotationSequence( subRotationSequence );
		RubiksCube::ConcatinateRotationSequence( rotationSequence, subRotationSequence );

		int centerCount = rubiksCube->SubCubeMatrixSize() - layerCount * 2;

		RubiksCube::Perspective perspective;

		perspective.rAxis = xAxis;
		perspective.uAxis = -zAxis;
		perspective.fAxis = yAxis;

		std::string rotationSequenceString;

		rotationSequenceString += CompoundRotate( "Fi", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Ui", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Fi", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Ui", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Fi", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Ui", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Fi", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Ui", layerCount, rubiksCube->SubCubeMatrixSize() - layerCount - 1 );

		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		rubiksCube->ParseRelativeRotationSequenceString( rotationSequenceString, relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, rotationSequence );
	}
	else if( layerCount <= rubiksCube->SubCubeMatrixSize() / 2 )
	{
		RubiksCube::Perspective perspective;

		perspective.rAxis = xAxis;
		perspective.uAxis = -zAxis;
		perspective.fAxis = yAxis;

		std::string rotationSequenceString;

		// To get a true spiral pattern, we would need to generalize this and keep going, but I'll stop here for now.

		rotationSequenceString += CompoundRotate( "L", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Di", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "D", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Di", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "D", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Di", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "D", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "Di", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "D", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
		rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";

		rotationSequenceString += CompoundRotate( "Li", 0, layerCount - 1 );

		RubiksCube::RelativeRotationSequence relativeRotationSequence;
		rubiksCube->ParseRelativeRotationSequenceString( rotationSequenceString, relativeRotationSequence );
		rubiksCube->TranslateRotationSequence( perspective, relativeRotationSequence, rotationSequence );
	}

	layerCount++;
	return true;
}

void CubeInACube::RotateEdgesOfCorner( const RubiksCube* rubiksCube, const RubiksCube::Perspective& cornerPerspectiveA, const RubiksCube::Perspective& cornerPerspectiveB, RubiksCube::RotationSequence& rotationSequence )
{
	RubiksCube::RelativeRotationSequence relativeRotationSequence;

	std::string rotationSequenceString;

	rotationSequenceString += CompoundRotate( "B", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "L", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Li", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Bi", 0, layerCount - 1 );

	rubiksCube->ParseRelativeRotationSequenceString( rotationSequenceString, relativeRotationSequence );
	rubiksCube->TranslateRotationSequence( cornerPerspectiveA, relativeRotationSequence, rotationSequence );

	rotationSequenceString = "";

	rotationSequenceString += CompoundRotate( "Bi", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "U", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "Ri", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "R", 0, layerCount - 1 ) + ", ";
	rotationSequenceString += CompoundRotate( "B", 0, layerCount - 1 );

	relativeRotationSequence.clear();
	rubiksCube->ParseRelativeRotationSequenceString( rotationSequenceString, relativeRotationSequence );
	rubiksCube->TranslateRotationSequence( cornerPerspectiveB, relativeRotationSequence, rotationSequence );
}

// CubeInACube.h