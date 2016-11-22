// Header.h

// Note that this program was written a long time ago, and at that time,
// I thought that creating a master header like this was a good idea.
// I know now, however, that it is a very bad idea, and makes compilation
// very, very slow.

#include <wx/setup.h>
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/colordlg.h>
#include <wx/glcanvas.h>
#include <wx/sizer.h>
#include <wx/timer.h>
#include <wx/textctrl.h>
#include <wx/xml/xml.h>
#include <wx/scopedptr.h>
#include <wx/image.h>
#include <wx/utils.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/arrstr.h>
#include <wx/filename.h>
#include <wx/base64.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <list>
#include <map>
#include <set>
#include <stdlib.h>

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
class CubeInACube;

#include "RubiksCube.h"
#include "Application.h"
#include "Frame.h"
#include "Canvas.h"
#include "Solver.h"
#include "SolverForCase2.h"
#include "SolverForCase3.h"
#include "SolverForCaseGreaterThan3.h"
#include "TriCycleSolver.h"
#include "CubeInACube.h"

// Header.h