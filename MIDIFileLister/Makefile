CC  = cc
CXX = c++

ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
	CXXFLAGS += -std=c++11
else
	CFLAGS += -g
	CXXFLAGS += -std=c++11
endif

OBJS = MIDIFileLister.o
DEPHEADERS = DirectoryLister.h

all:	MIDIFileLister.exe

MIDIFileLister.exe:	$(OBJS)
	$(CXX) -o $@ $^

%.o: 	%.cpp $(DEPHEADERS)
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) -o $@ $<

#%.o:	$(PROJECT_ROOT)%.cpp
#	$(CXX) -c $(CFLAGS) $(CXXFLAGS) -o $@ $<
#

%.o: 	%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr MIDIFileLister.exe build $(OBJS)

demo: MIDIFileLister.exe
	./MIDIFileLister.exe ../smf ".*\\.mid"
	