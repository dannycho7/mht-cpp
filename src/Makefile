CXX=g++
CXXFLAGS=-std=c++14 -Wall
GTEST_FLAGS=-lgtest
GTEST_MAIN=./googletest/googletest/src/gtest_main.cc


main: MerkleTree.o
MerkleTree.o: ./MerkleTree.cpp ./MerkleTree.hpp
	$(CXX) $(CXXFLAGS) -c ./MerkleTree.cpp -o ./MerkleTree.o
mhtc.so: ./MerkleTreeC.cpp ./MerkleTree.hpp MerkleTree.o
	$(CXX) $(CXXFLAGS) -shared ./MerkleTreeC.cpp ./MerkleTree.o -o ./mhtc.so
# GTEST
MerkleTreeTest.out: MerkleTreeTest.cpp MerkleTree.o
	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) $(GTEST_MAIN) MerkleTreeTest.cpp MerkleTree.o -o MerkleTreeTest.out
test: MerkleTreeTest.out
	./MerkleTreeTest.out
clean:
	rm -f *.so *.o *.out