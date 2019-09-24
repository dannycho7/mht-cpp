#include "MerkleTree.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

using std::pair;

MerkleTree::MerkleTree(const map<string, string> &rawData,
                       std::function<string(string)> hashFunc)
    : mHashFunc(hashFunc) {
    if (rawData.empty()) {
        throw std::invalid_argument("Must have at least one tuple.");
    }
    {
        int i = 0;
        std::transform(rawData.begin(), rawData.end(),
                       std::inserter(kmd_map, kmd_map.begin()),
                       [&i](pair<string, string> kd_pair) {
                           return pair<string, std::shared_ptr<MerkleData>>(
                               kd_pair.first, std::make_shared<MerkleData>(
                                                  kd_pair.second, i++));
                       });
    }
    vector<std::shared_ptr<MerkleNode>> level_nodes(kmd_map.size());
    int num_nodes_in_level = level_nodes.size();
    std::transform(kmd_map.begin(), kmd_map.end(), level_nodes.begin(),
                   [this](pair<string, std::shared_ptr<MerkleNode>> kmd_pair) {
                       auto md = kmd_pair.second;
                       auto mn = std::make_shared<MerkleNode>(
                           mHashFunc(md->val), md->mRelI);
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
            auto mn = std::make_shared<MerkleNode>(mHashFunc(concat), i);
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

string MerkleTree::getRoot() const {
    std::shared_ptr<MerkleNode> curr = kmd_map.begin()->second;
    while (curr->parent) curr = curr->parent;
    return curr->val;
}

VO MerkleTree::getVO(const string &k) const {
    auto md = findByKey(k);
    VO verification_obj;
    computeVOForMerkleData(md, verification_obj);
    return verification_obj;
}

void MerkleTree::computeVOForMerkleData(std::shared_ptr<const MerkleData> md,
                                        VO &verif_obj) const {
    verif_obj.val = md->val;
    std::shared_ptr<MerkleNode> curr_mn = md->parent;
    while (curr_mn->parent != NULL) {
        verif_obj.sibling_path.push_back(curr_mn->sibling.lock()->val);
        curr_mn = curr_mn->parent;
    }
}

std::shared_ptr<MerkleData> MerkleTree::findByKey(const string &k) const {
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
    auto md = md_it->second;
    md->val = v;
    std::shared_ptr<MerkleNode> prev = md;
    std::shared_ptr<MerkleNode> curr = md->parent;
    while (curr) {
        auto sibling = prev->sibling.lock();
        string concat = sibling ? (prev->mRelI < sibling->mRelI)
                                      ? prev->val + sibling->val
                                      : sibling->val + prev->val
                                : prev->val;
        curr->val = mHashFunc(concat);
        prev = curr;
        curr = curr->parent;
    }
}
