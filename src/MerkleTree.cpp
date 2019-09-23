#include "./MerkleTree.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <sstream>
#include <stdexcept>

MerkleTree::MerkleTree(const vector<Tuple> &tuples,
                       std::function<string(string)> hashFunc)
    : data(tuples.size()), mHashFunc(hashFunc) {
    assert(tuples.size() > 0);
    for (auto it = tuples.begin(); it != tuples.end(); it++) {
        assert(this->kd_map.find(it->first) == this->kd_map.end());
        this->kd_map[it->first] = new MerkleData(it->second);
    }

    std::transform(this->kd_map.begin(), this->kd_map.end(), data.begin(),
                   [](KDPair kd_pair) { return kd_pair.second; });

    vector<MerkleNode *> level_nodes(data.size());
    int num_nodes_in_level = level_nodes.size();
    std::transform(data.begin(), data.end(), level_nodes.begin(),
                   [this](MerkleData *md) {
                       MerkleNode *mn = new MerkleNode(mHashFunc(md->val));
                       md->parent = mn;
                       return mn;
                   });

    while (num_nodes_in_level > 1) {
        int num_nodes_next_level = ceil(num_nodes_in_level / 2);
        for (int i = 0; i < num_nodes_next_level; i++) {
            string concat = level_nodes[2 * i]->val;
            const bool has_right_neighbor = (2 * i + 1 < num_nodes_in_level);
            if (has_right_neighbor) concat += level_nodes[2 * i + 1]->val;
            MerkleNode *mn = new MerkleNode(mHashFunc(concat));
            level_nodes[2 * i]->parent = mn;
            if (has_right_neighbor) {
                level_nodes[2 * i + 1]->parent = mn;
                level_nodes[2 * i + 1]->sibling = level_nodes[2 * i];
                level_nodes[2 * i]->sibling = level_nodes[2 * i + 1];
            }
            level_nodes[i] = mn;
        }
        num_nodes_in_level = num_nodes_next_level;
    }
}

MerkleTree::~MerkleTree() {
    for (auto it = this->data.begin(); it != this->data.end(); it++) {
        delete *it;
    }
}

string MerkleTree::getRoot() const {
    MerkleNode *curr = this->data[0];
    while (curr->parent != NULL) curr = curr->parent;
    return curr->val;
}

VO MerkleTree::getVO(const string &k) const {
    MerkleData *md = findByKey(k);
    VO verification_obj;
    computeVOForMerkleData(md, verification_obj);
    return verification_obj;
}

void MerkleTree::computeVOForMerkleData(const MerkleData *const md,
                                        VO &verif_obj) const {
    verif_obj.val = md->val;
    MerkleNode *curr_mn = md->parent;
    while (curr_mn->parent != NULL) {
        verif_obj.sibling_path.push_back(curr_mn->sibling->val);
        curr_mn = curr_mn->parent;
    }
}

MerkleData *MerkleTree::findByKey(const string &k) const {
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
    MerkleData *md = md_it->second;
    md->val = v;
    MerkleNode *prev = md;
    MerkleNode *curr = md->parent;
    while (curr != NULL) {
        string concat = (prev->sibling != NULL) ? prev->val + prev->sibling->val
                                                : prev->val;
        curr->val = mHashFunc(concat);
        prev = curr;
        curr = curr->parent;
    }
}
