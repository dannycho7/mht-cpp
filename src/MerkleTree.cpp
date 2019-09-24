#include "MerkleTree.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <iostream>

using std::pair;

MerkleTree::MerkleTree(const map<string, string> &rawData,
                       std::function<string(string)> hashFunc)
    : mHashFunc(hashFunc) {
    if (rawData.empty()) {
        throw std::invalid_argument("Must have at least one tuple.");
    }

    int i = 0;
    std::transform(rawData.begin(), rawData.end(),
                   std::inserter(kmd_map, kmd_map.begin()),
                   [&i](pair<string, string> kd_pair) {
                       return pair<string, MerkleData *>(
                           kd_pair.first, new MerkleData(kd_pair.second, i++));
                   });

    vector<MerkleNode *> level_nodes(kmd_map.size());
    int num_nodes_in_level = level_nodes.size();
    std::transform(kmd_map.begin(), kmd_map.end(), level_nodes.begin(),
                   [this](pair<string, MerkleData *> kmd_pair) {
                       auto md = kmd_pair.second;
                       MerkleNode *mn =
                           new MerkleNode(mHashFunc(md->val), md->mRelI);
                       md->parent = mn;
                       return mn;
                   });

    while (num_nodes_in_level > 1) {
        int num_nodes_next_level =
            ceil(static_cast<double>(num_nodes_in_level) / 2);
        for (int i = 0; i < num_nodes_next_level; i++) {
            string concat = level_nodes[2 * i]->val;
            const bool has_right_neighbor = (2 * i + 1 < num_nodes_in_level);
            if (has_right_neighbor) concat += level_nodes[2 * i + 1]->val;
            MerkleNode *mn = new MerkleNode(mHashFunc(concat), i);
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
    for (auto it = kmd_map.begin(); it != kmd_map.end(); it++) {
        delete it->second;
    }
}

string MerkleTree::getRoot() const {
    MerkleNode *curr = kmd_map.begin()->second;
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
    auto md_it = this->kmd_map.find(k);
    if (md_it == this->kmd_map.end()) {
        std::ostringstream err_msg_s;
        err_msg_s << k << " does not exist in the MerkleTree";
        throw std::invalid_argument(err_msg_s.str());
    }
    return md_it->second;
}

void MerkleTree::update(string k, string v) {
    auto md_it = this->kmd_map.find(k);
    if (md_it == this->kmd_map.end()) {
        throw std::invalid_argument("Invalid key: " + k);
    }
    MerkleData *md = md_it->second;
    md->val = v;
    MerkleNode *prev = md;
    MerkleNode *curr = md->parent;
    while (curr != NULL) {
        string concat = (prev->sibling != NULL)
                            ? (prev->mRelI < prev->sibling->mRelI)
                                  ? prev->val + prev->sibling->val
                                  : prev->sibling->val + prev->val
                            : prev->val;
        curr->val = mHashFunc(concat);
        prev = curr;
        curr = curr->parent;
    }
}
