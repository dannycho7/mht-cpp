#include <algorithm>
#include <cassert>
#include <cmath>
#include "./MerkleTree.hpp"
#include "./PicoSHA2/picosha2.h"

string hash_256(string in) {
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
