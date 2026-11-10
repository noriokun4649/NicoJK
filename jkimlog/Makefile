CXXFLAGS := -std=c++11 -Wall -Wextra -pedantic-errors -O2 $(CXXFLAGS)
LDFLAGS := -Wl,-s $(LDFLAGS)

all: jkimlog
jkimlog: jkimlog.cpp ../ImportLogUtil.cpp $(wildcard ../*.h)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TARGET_ARCH) -o $@ $(filter-out %.h, $^)
clean:
	$(RM) jkimlog
