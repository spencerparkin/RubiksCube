# Makefile for RubiksCube on Linux

SRC = \
	Code/Application.cpp \
	Code/Canvas.cpp \
	Code/CubeInACube.cpp \
	Code/Frame.cpp \
	Code/RubiksCube.cpp \
	Code/Solver.cpp \
	Code/SolverForCase2.cpp \
	Code/SolverForCase3.cpp \
	Code/SolverForCaseGreaterThan3.cpp \
	Code/TriCycleSolver.cpp \
	Code/c3ga/c3ga.cpp \
	Code/c3ga/c3ga_parse_mv.cpp

OBJ = $(SRC:.cpp=.o)

%.o: %.cpp
	g++ -g -c $< -o $@ `wx-config --cxxflags` -std=c++11

RubiksCube: $(OBJ)
	g++ -g -o RubiksCube $(OBJ) -lGL -lGLU -lglut `wx-config --libs` -lwx_gtk2u_gl-3.0
	export UBUNTU_MENUPROXY=0

all: RubiksCube

clean:
	rm -f $(OBJ) RubiksCube
