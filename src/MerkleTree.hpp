#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#include <string>
#include <vector>

using std::string;
using std::vector;

struct MerkleNode {
	string val;
	MerkleNode* parent;
	MerkleNode* sibling;
	MerkleNode(string val): val(val), parent(NULL), sibling(NULL) {}
};

struct MerkleData : MerkleNode {
	MerkleData(string val) : MerkleNode(val) {}
};

class MerkleTree {
public:
	MerkleTree(const vector<string>& tuples);
	string getRoot() const { return this->root; }
private:
	string root;
	vector<MerkleData*> data;
};
#endif