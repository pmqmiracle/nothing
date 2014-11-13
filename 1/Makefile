INCFLAGS  = -I /usr/include/GL
INCFLAGS += -I ./include/vecmath

LINKFLAGS = -lglut -lGL -lGLU
#LINKFLAGS += -L /mit/6.837/public/lib -lvecmath

CFLAGS    = -O2 -Wall -ansi -fpermissive#-DSOLN
CC        = g++
SRCS      = main.cpp parse.cpp curve.cpp surf.cpp camera.cpp ./vecmath/*.cpp
OBJS      = $(SRCS:.cpp=.o)
PROG      = Assignment1

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)
