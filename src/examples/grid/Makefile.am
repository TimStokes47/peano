AUTOMAKE_OPTIONS = subdir-objects

PROJECT_NAME = grid
PEANO_ROOT   = /home/tobias/git/Peano/src
OBJS        += examples/grid/main.o
OBJS        += examples/grid/MyObserver.o
DIMENSIONS   = 2

-include $(PEANO_ROOT)/peano4/Makefile
-include $(PEANO_ROOT)/tarch/Makefile


ifeq ($(BUILD_MODE),release)
	CFLAGS += --std=c++14 -O3 
else ifeq ($(BUILD_MODE),profile)
	CFLAGS += --std=c++14 -O3 -DPeanoDebug=0 -g
else 
	CFLAGS += --std=c++14 -g -O3 -DPeanoDebug=2
endif


CFLAGS += -DSharedCPP
#CFLAGS += -DSharedTBB -I/opt/intel/tbb/include
LFLAGS += -lpthread
#LFLAGS += -lpthread -L/opt/intel/tbb/lib/intel64/gcc4.7 -ltbb


all:	compile

compile:	$(OBJS)
	$(CXX) -o Peano-$(PROJECT_NAME) $(LFLAGS) $^

%.o:	%.cpp
	$(CXX) -DDimensions=$(DIMENSIONS) -I$(PEANO_ROOT) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(OBJS)
