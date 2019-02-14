INCLUDES =
LDFLAGS  =
CFLAGS   = -O2
CXXFLAGS   = -O2 -std=c++11
CXX      = g++
CC       = gcc

SRCDIR = $HOME/opt/ToySQL
CATA = cata.o
BUFFER = buffer.o
RECORDER = recorder.o


examples = toySQL
dep = cata buffer recorder

CC_COMPILE = $(CC) $(CFLAGS) $(INCLUDES)
CXX_COMPILE = $(CXX) $(CFLAGS) $(INCLUDES)

all: $(examples)

toySQL: cata buffer recorder
	$(CXX_COMPILE) -o toySQL  $(CXXFLAGS) $(CATA) $(BUFFER) $(RECORDER)
	Interpreter/scanner.cpp Interpreter/parser.cpp Interpreter/main.cpp
cata:
	$(CXX_COMPILE) -c -o $(CATA) $(CXXFLAGS) Catalog/catalog.cpp
buffer:
	$(CXX_COMPILE) -c -o $(BUFFER) $(CXXFLAGS) Buffer/buffer.cpp
recorder:
	$(CXX_COMPILE)  -c -o $(RECORDER) $(CXXFLAGS) Recorder/recorder.cpp

clean:
	-rm -f $(dep).o
