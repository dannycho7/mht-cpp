#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <string>
#include <vector>

class MerkleTree {
public:
	MerkleTree(const std::vector<std::string>& data);
private:
	std::string root;
	std::vector<std::string> data;
};
#endif