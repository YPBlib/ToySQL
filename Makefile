INCLUDES =
LDFLAGS  =
CFLAGS   = -O2
CXXFLAGS   = -O2 -std=c++11
CXX      = g++
CC       = gcc

SRCDIR = $HOME/opt/ToySQL
CATA= cata.o


examples = toySQL

CC_COMPILE = $(CC) $(CFLAGS) $(INCLUDES)
CXX_COMPILE = $(CXX) $(CFLAGS) $(INCLUDES)

all: $(examples)

toySQL: cata
	$(CXX_COMPILE) -o toySQL  $(CXXFLAGS) $(CATA) Interpreter/main.cpp
cata:
	$(CXX_COMPILE) -c -o cata.o $(CXXFLAGS) catalog/catalog.cpp

clean:
	-rm -f $(examples).o $(examples)
