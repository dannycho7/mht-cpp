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

struct VO {
	string val;
	vector<string> sibling_path;
};

class MerkleTree {
public:
	MerkleTree(const vector<string>& tuples);
	string getRoot() const { return this->root; }
	VO getVO(const string val) const;
private:
	string root;	
	vector<MerkleData*> data;
	void computeVOForMerkleData(const MerkleData* md, VO& verif_obj) const;
	MerkleData* findByVal(const string val) const;
};
#endif