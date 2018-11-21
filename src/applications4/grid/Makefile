PROJECT_NAME = grid
PEANO_ROOT   = /home/tobias/git/Peano/src
OBJS         = applications4/grid/main.o
DIMENSIONS   = 2

-include $(PEANO_ROOT)/peano4/Makefile
-include $(PEANO_ROOT)/tarch/Makefile


ifeq ($(BUILD_MODE),release)
	CFLAGS += --std=c++14 -O3 
else 
	CFLAGS += --std=c++14 -g -O3 -DPeanoDebug=1
endif


all:	compile

compile:	$(OBJS)
	$(CXX) -o Peano-$(PROJECT_NAME) $^

%.o:	%.cpp
	$(CXX) -DDimensions=$(DIMENSIONS) -I$(PEANO_ROOT) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(OBJS)
