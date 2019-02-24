#include <algorithm>
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <sstream>
#include "./MerkleTree.hpp"
#include "./PicoSHA2/picosha2.h"

string hash_256(const string& in) {
	string hash_hex_str;
	picosha2::hash256_hex_string(in, hash_hex_str); 
	return hash_hex_str;
}

MerkleTree::MerkleTree(const vector<Tuple>& tuples): data(tuples.size()) {
	assert(tuples.size() > 0);
	for (auto it = tuples.begin(); it != tuples.end(); it++) {
		assert(this->kd_map.find(it->first) != this->kd_map.end());
		this->kd_map[it->first] = new MerkleData(it->second);
	}

	std::transform(this->kd_map.begin(), this->kd_map.end(), data.begin(), [](KDPair kd_pair) {
		return kd_pair.second;
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

MerkleTree::~MerkleTree() {
	for (auto it = this->data.begin(); it != this->data.end(); it++) {
		delete *it;
	}
}

VO MerkleTree::getVO(const string& k) const {
	MerkleData* md = findByKey(k);
	VO verification_obj;
	computeVOForMerkleData(md, verification_obj);
	return verification_obj;
}

void MerkleTree::computeVOForMerkleData(const MerkleData* const md, VO& verif_obj) const {
	verif_obj.val = md->val;
	MerkleNode* curr_mn = md->parent;
	while (curr_mn->parent != NULL) {
		verif_obj.sibling_path.push_back(curr_mn->sibling->val);
		curr_mn = curr_mn->parent;
	}
}

MerkleData* MerkleTree::findByKey(const string& k) const {
	auto md_it = this->kd_map.find(k);
	if (md_it == this->kd_map.end()) {
		std::ostringstream err_msg_s;
		err_msg_s << k << " does not exist in the MerkleTree";
 		throw std::invalid_argument(err_msg_s.str());
	}
	return md_it->second;
}

void MerkleTree::update(string k, string v) {
	auto md_it = this->kd_map.find(k);
	assert(md_it != this->kd_map.end());
	MerkleData* md = md_it->second;
	md->val = v;
	MerkleNode* prev = md;
	MerkleNode* curr = md->parent;
	while (curr != NULL) {
		string concat = (prev->sibling != NULL) ? prev->val + prev->sibling->val : prev->val;
		curr->val = hash_256(concat);
		prev = curr;
		curr = curr->parent;
	}
}
