#include <algorithm>
#include <cmath>
#include "./MerkleTree.hpp"
#include "./PicoSHA2/picosha2.h"

std::string hash_256(std::string in) {
	std::string hash_hex_str;
	picosha2::hash256_hex_string(in, hash_hex_str); 
	return hash_hex_str;
}

MerkleTree::MerkleTree(const std::vector<std::string>& data): data(data) {
	std::vector<std::string> level_nodes(data.size()); 
	int num_nodes_in_level = level_nodes.size();
	std::transform(data.begin(), data.end(), level_nodes.begin(), hash_256);

	while (num_nodes_in_level > 1) {
		for (int i = 0; i < num_nodes_in_level; i+=2) {
			std::string concat = level_nodes[2*i];
			if (2 * i + 1 < num_nodes_in_level)
				concat += level_nodes[2*i + 1];
			level_nodes[i] = hash_256(concat);
		}
		num_nodes_in_level = ceil(num_nodes_in_level / 2);
	}
	this->root = level_nodes[0];
}