TARGETS = list

#ARCH = -m32
#CXXBASE = clang++
CXXBASE = g++
CXX = $(CXXBASE) $(ARCH) 
CC = $(CXX)
CXXFLAGS = -ggdb -O -Wall -Werror 

OBJS = list.o


all: $(TARGETS)

$(OBJS): $(HEADERS)

clean:
	rm -f $(TARGETS) $(OBJS) *.s *~ .*~

.SUFFIXES: .cc

.PHONY: all clean

