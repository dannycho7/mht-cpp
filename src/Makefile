CXXFLAGS=-std=c++11 -Wall

main: MerkleTree.o
MerkleTree.o: ./MerkleTree.cpp ./MerkleTree.hpp
	$(CXX) $(CXXFLAGS) -c ./MerkleTree.cpp -o ./MerkleTree.o
mhtc.so: ./MerkleTreeC.cpp ./MerkleTree.hpp MerkleTree.o
	$(CXX) $(CXXFLAGS) -shared ./MerkleTreeC.cpp ./MerkleTree.o -o ./mhtc.so