#pragma once

#include <map>
#include <string>
#include <utility> /* std::pair */
#include <vector>

using std::map;
using std::pair;
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

typedef pair<string, string> Tuple; /* k, v */
typedef pair<string, MerkleData*> KDPair;

class MerkleTree {
public:
	MerkleTree(const vector<Tuple>& tuples);
	~MerkleTree();
	string getRoot() const { return this->root; }
	VO getVO(const string& val) const;
private:
	string root;
	map<string, MerkleData*> kd_map;
	vector<MerkleData*> data;
	void computeVOForMerkleData(const MerkleData* const md, VO& verif_obj) const;
	MerkleData* findByVal(const string& val) const;
};
