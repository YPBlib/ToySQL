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

target: 
	$(CXX_COMPILE) -o toySQL   $(CXXFLAGS) \
	Buffer/buffer.cpp Catalog/catalog.cpp Recorder/recorder.cpp Interpreter/common.cpp \
	Interpreter/scanner.cpp Interpreter/parser.cpp Interpreter/main.cpp


toySQL: cata buffer recorder
	$(CXX_COMPILE) -o toySQL  $(CXXFLAGS) $(CATA) $(BUFFER) $(RECORDER)
	Interpreter/scanner.cpp Interpreter/parser.cpp Interpreter/main.cpp
cata:
	$(CXX_COMPILE)  -o $(CATA)  $(CXXFLAGS) Catalog/catalog.cpp Interpreter/common.cpp
buffer:
	$(CXX_COMPILE) -o $(BUFFER)  $(CXXFLAGS) Buffer/buffer.cpp Interpreter/common.cpp
recorder:
	$(CXX_COMPILE)   -o $(RECORDER)  $(CXXFLAGS) Recorder/recorder.cpp Interpreter/common.cpp

clean:
	-rm -f $(dep).o
