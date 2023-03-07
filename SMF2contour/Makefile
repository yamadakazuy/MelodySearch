#PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++ -std=c++2a -Wall 

CFLAGS += -O2
CXXFLAGS +=  -I./

OBJS = main.o smf.o

CFLAGS += -g

EXECUTABLE = smf2cont.exe

all:	$(EXECUTABLE)

$(EXECUTABLE):	$(OBJS)
	$(CXX) -o $@ $^

%.o:	%.cpp 
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(EXECUTABLE) $(OBJS)
	rm -f *~
