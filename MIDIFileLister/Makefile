PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

OBJS = MIDIFileLister.o

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
	CXXFLAGS += -std=c++1
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
	CXXFLAGS += -std=c++11
else
	$(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	MIDIFileLister

MIDIFileLister:	$(OBJS)
	$(CXX) -o $@ $^

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr MIDIFileLister $(OBJS)
