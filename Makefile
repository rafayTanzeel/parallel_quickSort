CXXFLAGS = -ggdb3 -m64 -std=c++0x -pthread -lrt -Wall -O3 ppp.C random.C TaskGroup.C
DEBUG  = -DPPP_DEBUG

all:
	$(CXX) $(CXXFLAGS) driver-sort.C -o driver-sort
	$(CXX) $(CXXFLAGS) driver-compute.C -o driver-compute
	$(CXX) $(CXXFLAGS) driver-reduce.C -o driver-reduce

clean:
	rm -f driver-sort driver-compute driver-reduce


