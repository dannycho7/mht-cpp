#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include "./MerkleTree.hpp"
#include "./PicoSHA2/picosha2.h"

string hash_256(const string in) {
	string hash_hex_str;
	picosha2::hash256_hex_string(in, hash_hex_str); 
	return hash_hex_str;
}

MerkleTree::MerkleTree(const vector<string>& tuples): data(tuples.size()) {
	assert(tuples.size() > 0);
	std::transform(tuples.begin(), tuples.end(), data.begin(), [](string t) {
		return new MerkleData(t);
	});

	vector<MerkleNode*> level_nodes(data.size());
	int num_nodes_in_level = level_nodes.size();
	std::transform(data.begin(), data.end(), level_nodes.begin(), [](MerkleData* md) {
		MerkleNode* mn = new MerkleNode(hash_256(md->val));
		md->parent = mn;
		return mn;
	});

	while (num_nodes_in_level > 1) {
		for (int i = 0; i < num_nodes_in_level; i+=2) {
			string concat = level_nodes[2*i]->val;
			const bool has_right_neighbor = (2 * i + 1 < num_nodes_in_level);
			if (has_right_neighbor)
				concat += level_nodes[2*i + 1]->val;
			MerkleNode* mn = new MerkleNode(hash_256(concat));
			level_nodes[2*i]->parent = mn;
			if (has_right_neighbor) {
				level_nodes[2*i + 1]->parent = mn;
				level_nodes[2*i + 1]->sibling = level_nodes[2*i];
				level_nodes[2*i]->sibling = level_nodes[2*i + 1];
			}
		}
		num_nodes_in_level = ceil(num_nodes_in_level / 2);
	}
	this->root = level_nodes[0]->val;
}

VO MerkleTree::getVO(const string val) const {
	MerkleData* md = findByVal(val);
	VO verification_obj;
	computeVOForMerkleData(md, verification_obj);
	return verification_obj;
}

void MerkleTree::computeVOForMerkleData(const MerkleData* md, VO& verif_obj) const {
	verif_obj.val = md->val;
	MerkleNode* curr_mn = md->parent;
	while (curr_mn->parent != NULL) {
		verif_obj.sibling_path.push_back(curr_mn->sibling->val);
		curr_mn = curr_mn->parent;
	}
}

MerkleData* MerkleTree::findByVal(const string val) const {
	auto it = std::find_if(this->data.begin(), this->data.end(), [val](MerkleData* md_ptr) -> bool {
		return (md_ptr->val == val);
	});
	if (it == this->data.end()) {
		std::ostringstream err_msg_s;
		err_msg_s << val << " does not exist in the MerkleTree";
 		throw std::invalid_argument(err_msg_s.str());
	}
	return *it;
}
