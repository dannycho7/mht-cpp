#define CAST_MHT(x) (static_cast<MerkleTree *>(x))
#include "./MerkleTree.hpp"

extern "C" {
	/*
	 * @param[in] raw_tuples a char* array where tuples occupy 2 contiguous spots
	 * @param[in] rt_size
	 * @param[out] MerkleTree instance
	 */
	void* mht_create(const char* raw_tuples[], int rt_size) {
		// convert to vector<Tuple>
		vector<Tuple> tuples;
		for (int i = 0; i < rt_size; i+=2) {
			tuples.push_back(Tuple(raw_tuples[i], raw_tuples[i+1]));
		}
		return new MerkleTree(tuples);
	}
}