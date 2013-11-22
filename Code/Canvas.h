// Canvas.h

//==================================================================================================
class Canvas : public wxGLCanvas
{
public:

	Canvas( wxWindow* parent );
	virtual ~Canvas( void );

	void AdjustSizeFor( const RubiksCube* rubiksCube );
	bool Animate( void );
	bool IsAnimating( double tolerance = 1e-7 ) const;
	bool ApplyRotation( const RubiksCube::Rotation& rotation );

private:

	void BindContext( void );

	void PreRender( GLenum mode );
	void PostRender( GLenum mode );

	void OnPaint( wxPaintEvent& event );
	void OnSize( wxSizeEvent& event );
	void OnMouseLeftDown( wxMouseEvent& event );
	void OnMouseRightDown( wxMouseEvent& event );
	void OnMouseRightUp( wxMouseEvent& event );
	void OnMouseMotion( wxMouseEvent& event );
	void OnMouseCaptureLost( wxMouseCaptureLostEvent& event );

	struct Camera
	{
		c3ga::vectorE3GA eye;
		c3ga::vectorE3GA xAxis;
		c3ga::vectorE3GA yAxis;
	};

	Camera camera;
	wxPoint mousePos;
	wxGLContext* context;
	static int attributeList[];
	RubiksCube::Rotation rotation;
	RubiksCube::Size size;
	RubiksCube::Grip grip;
	bool grippingCube;
	unsigned int* hitBuffer;
	int hitBufferSize;
};

// Canvas.h