INCLUDES =
LDFLAGS  =
CFLAGS   = -O2
CXXFLAGS   = -O2 -std=c++11
CXX      = g++
CC       = gcc

SRCDIR = $HOME/opt/ToySQL
CATA = cata.o
BUFFER = buffer.o


examples = toySQL

CC_COMPILE = $(CC) $(CFLAGS) $(INCLUDES)
CXX_COMPILE = $(CXX) $(CFLAGS) $(INCLUDES)

all: $(examples)

toySQL: cata buffer
	$(CXX_COMPILE) -o toySQL  $(CXXFLAGS) $(CATA) Interpreter/scanner.cpp Interpreter/parser.cpp Interpreter/main.cpp
cata:
	$(CXX_COMPILE) -c -o $(CATA) $(CXXFLAGS) catalog/catalog.cpp
buffer:
	$(CXX_COMPILE) -c -o $(BUFFER) $(CXXFLAGS) buffer/buffer.cpp

clean:
	-rm -f $(examples).o $(examples)
