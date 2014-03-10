CXXFLAGS+=-Wall -Werror -Wextra -Weffc++ \
	    -Wstrict-aliasing -pedantic -std=c++11 -g -O0 -fno-omit-frame-pointer
LDLIBS+=

CXX=clang++

SRCS=list.cpp
EXE=list
DEPS=.depend

OBJS=$(patsubst %.cpp, %.o, $(SRCS))

build: depend $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(LDFLAGS) $(LDLIBS)

depend: $(ALL_SRCS)
	$(CXX) $(CXXFLAGS) -MM $(SRCS) > $(DEPS)

clean:
	$(RM) $(RMFLAGS) $(EXE) $(OBJS) $(DEPS)

-include $(DEPS)
