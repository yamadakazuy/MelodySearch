#PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++ -std=c++17 -Wall 

CFLAGS += -O0
CXXFLAGS +=  -I./

OBJS = main.o smf.o

CFLAGS += -g

EXECUTABLE = smfscoreviewer.exe

all:	$(EXECUTABLE)

$(EXECUTABLE):	$(OBJS)
	$(CXX) -o $@ $^

%.o:	src/%.cpp 
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	src/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(EXECUTABLE) $(OBJS)
	rm -f *~
