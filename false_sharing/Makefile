TARGETS := Sharing
CXX     := g++
CXXFLAGS:= -std=c++17 -ggdb -O -Wall -Werror -pthread
LDFLAGS := -pthread

OBJS := sharing.o

all: $(TARGETS)

Sharing: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@

clean:
	rm -f $(OBJS) $(TARGETS)

