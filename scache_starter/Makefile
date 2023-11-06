TARGETS = run_tests

CXXBASE = g++
CXX = $(CXXBASE) $(ARCH) -std=c++2a
CC = $(CXX)
CXXFLAGS = -ggdb -O -Wall -Werror 

OBJS = softram.o cache.o testing.o
HEADERS = softram.hh cache.hh

all: $(TARGETS)

$(OBJS): $(HEADERS)

run_tests: $(OBJS) $(LIB)
	$(CXX) -o $@ $^

clean:
	rm -f $(TARGETS) $(OBJS) *.s *~ .*~

.SUFFIXES: .cc

.PHONY: all clean

