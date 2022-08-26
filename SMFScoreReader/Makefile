#PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++ -std=c++17 -Wall 

CFLAGS += -O0
CXXFLAGS +=  -I./

OBJS = main.o smf.o

CFLAGS += -g

EXECUTABLE = demo.exe

all:	$(EXECUTABLE)

$(EXECUTABLE):	$(OBJS)
	$(CXX) -o $@ $^

#%.o:	$(PROJECT_ROOT)%.cpp
%.o:	%.cpp 
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

#%.o:	$(PROJECT_ROOT)%.c
%.o:	%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(EXECUTABLE) $(OBJS)
	rm -f *~
