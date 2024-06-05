# Compiler
CXX = g++

# Compiler flags
CFLAGS = -O3 -std=c++11 -I./ -I/opt/homebrew/Cellar/sfml/2.6.1/include

# Linker flags
LDFLAGS = -L/opt/homebrew/Cellar/sfml/2.6.1/lib

# SFML Libraries
SFML_LIBS = -lsfml-window -lsfml-graphics -lsfml-system

# Targets
all: nbody-nographics nbody-graphics

nbody-nographics: nbody.cpp
	${CXX} ${CFLAGS} nbody.cpp -o nbody-nographics -lpthread

nbody-graphics: nbody.cpp
	${CXX} ${CFLAGS} nbody.cpp -o nbody-graphics ${LDFLAGS} ${SFML_LIBS} -DGRAPHICS -lpthread

clean:
	rm -f nbody-graphics nbody-nographics
