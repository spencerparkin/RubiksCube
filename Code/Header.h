// Header.h

#include <wx/setup.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/glcanvas.h>
#include <wx/sizer.h>
#include <wx/timer.h>
#include <wx/textctrl.h>
#include <wx/xml/xml.h>
#include <wx/scopedptr.h>
#include <wx/image.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <list>

#include "c3ga/c3ga.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

class RubiksCube;
class Application;
class Frame;
class Canvas;
class Solver;
class SolverForCase2;
class SolverForCase3;
class SolverForCaseGreaterThan3;
class TriCycleSolver;

#include "RubiksCube.h"
#include "Application.h"
#include "Frame.h"
#include "Canvas.h"
#include "Solver.h"
#include "SolverForCase2.h"
#include "SolverForCase3.h"
#include "SolverForCaseGreaterThan3.h"
#include "TriCycleSolver.h"

// Header.h