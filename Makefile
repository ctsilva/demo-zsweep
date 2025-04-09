#SHELL=/bin/csh
#CXX = CC
CXX = g++

CXXFLAGS    = -O3 -Wall $(INCLUDES) -D_BSD_SOURCE -DLINUX
#CXXFLAGS    = -O3 -fprofile-arcs -ftest-coverage $(INCLUDES) -D_BSD_SOURCE

INCLUDES = -I/usr/freeware/include
LIBDIRS =  -L/usr/freeware/lib32
LIBS = -L/usr/X11R6/lib -lglut -lGL -lGLU -lXext -lXmu -lX11 -lm -lXi

TK_FLAGS =

#### Shouldn't have to change anything beyond this point ####


OBJS = zs_ZSweepDemo.o

OBJS = zs_general.o \
       zs_basic.o zs_cell.o zs_heap.o zs_screenLists.o zs_hash.o \
       zs_debugFuncs.o zs_readToff.o zs_boundary.o zs_sparse.o   \
       zs_scene.o zs_render.o zs_renderPAR.o zs_fakeSMP.o zs_OpenGL.o \
       zs_mainOriginal.o zs_ZSweepDemo.o

# Link stage -------------------
demo: $(OBJS)
	$(CXX) $(CXXFLAGS) -o demo $(OBJS) $(LIBDIRS) $(LIBS)

zs_ZSweepDemo.o: zs_ZSweepDemo.cpp zs_ZSweepDemo.hh
	$(CXX) $(CXXFLAGS)  -c zs_ZSweepDemo.cpp

clean:
	rm -f core *.o demo *~

