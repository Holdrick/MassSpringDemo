EXEFILE = asg3
LIBS = -lglfw -lGLEW -lGL
LIBDIRS += -L/usr/X11R6/lib -L/usr/X11R6/lib64 -L/usr/local/lib
INCDIRS += -I/usr/include -I/usr/local/include -I/usr/include/GL


$(EXEFILE): main.o MassSpring.o GOBJ.o
	g++ -o $(EXEFILE) $(LIBDIRS) $(INCDIRS) $(LIBS) main.o MassSpring.o GOBJ.o

main.o: main.cpp MassSpring.h GOBJ.h Particle.h Spring.h
	g++ -c main.cpp

MassSpring.o: MassSpring.cpp MassSpring.h GOBJ.h Particle.h Spring.h
	g++ -c MassSpring.cpp

GOBJ.o: GOBJ.cpp GOBJ.h
	g++ -c GOBJ.cpp 

clean:
	rm $(EXEFILE) *.o *# *~
