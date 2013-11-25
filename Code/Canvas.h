// Canvas.h

//==================================================================================================
class Canvas : public wxGLCanvas
{
public:

	Canvas( wxWindow* parent );
	virtual ~Canvas( void );

	enum Projection
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
	};

	void ShowPerspectiveLabels( bool showPerspectiveLabels );
	bool ShowPerspectiveLabels( void ) const;

	void SetProjection( Projection projection );
	Projection GetProjection( void ) const;

	void AdjustSizeFor( const RubiksCube* rubiksCube );
	bool Animate( void );
	bool IsAnimating( double tolerance = 1e-7 ) const;
	bool ApplyRotation( const RubiksCube::Rotation& rotation );
	void DeterminePerspective( c3ga::vectorE3GA& rAxis, c3ga::vectorE3GA& uAxis, c3ga::vectorE3GA& fAxis ) const;

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

	typedef std::list< c3ga::vectorE3GA > AxisList;

	static void SortAxesByBlade( const AxisList& axisList, const c3ga::bivectorE3GA& blade,
											AxisList* backAxisList,
											AxisList* frontAxisList,
											AxisList* neitherAxisList );

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
	Projection projection;
	bool showPerspectiveLabels;
};

// Canvas.h