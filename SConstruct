# SConstruct for RubiksCube program

import os

obj_env = Environment( parse_flags = '!wx-config --cxxflags' )
obj_env.Append( CCFLAGS = '--std=c++11' )

if not obj_env.GetOption( 'clean' ):
	conf = Configure( obj_env )

	if not conf.CheckLib( 'wx_baseu-3.0' ):
		print( 'wxWidgets may not be installed; couldn\'t find it.' )
		Exit(1)

	if not conf.CheckCXXHeader( 'boost/tokenizer.hpp' ) or not conf.CheckCXXHeader( 'boost/algorithm/string.hpp' ):
		print( 'Boost may not be installed; couldn\'t find required headers.' )
		Exit(1)

	obj_env = conf.Finish()

cpp_source_list = Glob( 'Code/*.cpp' ) + Glob( 'Code/c3ga/*.cpp' )
h_source_list = Glob( 'Code/*.h' ) + Glob( 'Code/c3ga/*.h' )
source_list = cpp_source_list + h_source_list

object_list = []
for source_file in cpp_source_list:
	object_file = obj_env.StaticObject( source_file )
	object_list.append( object_file )

prog_env = Environment( PROGNAME = 'RubeCube', parse_flags = '!wx-config --libs core base adv xml gl' )
prog_env.Append( LIBS = '-lGL' )
prog_env.Append( LIBS = '-lGLU' )
prog = prog_env.Program( '$PROGNAME', source = object_list )

dest_dir = '/usr'
if 'DESTDIR' in os.environ:
	dest_dir = os.environ[ 'DESTDIR' ]

install_env = Environment(
	BIN = dest_dir + '/bin',
	SHARE = dest_dir + '/share' )

texture_list = Glob( 'Textures/*.png' )

install_env.Install( '$SHARE/RubiksCube/Textures', texture_list )
install_env.Install( '$BIN', prog )
install_env.Alias( 'install', [ '$BIN', '$SHARE/RubiksCube/Textures' ] )

