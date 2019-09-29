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

    std::transform(rawData.begin(), rawData.end(),
                   std::inserter(mMerkleDataByKey, mMerkleDataByKey.begin()),
                   [i = 0](pair<string, string> kdPair) mutable {
                       return pair<string, std::shared_ptr<MerkleData>>(
                           kdPair.first,
                           std::make_shared<MerkleData>(kdPair.second, i++));
                   });

    vector<std::shared_ptr<MerkleNode>> levelNodes(mMerkleDataByKey.size());
    int numNodesInLevel = levelNodes.size();
    std::transform(
        mMerkleDataByKey.begin(), mMerkleDataByKey.end(), levelNodes.begin(),
        [this](pair<string, std::shared_ptr<MerkleNode>> kmdPair) {
            auto md = kmdPair.second;
            auto mn =
                std::make_shared<MerkleNode>(mHashFunc(md->val), md->mRelI);
            md->parent = mn;
            return mn;
        });

    while (numNodesInLevel > 1) {
        int numNodesNextLevel =
            ceil(static_cast<double>(numNodesInLevel) / 2);
        for (int i = 0; i < numNodesNextLevel; i++) {
            string concat = levelNodes[2 * i]->val;
            const bool hasRightNeighbor = (2 * i + 1 < numNodesInLevel);
            if (hasRightNeighbor) concat += levelNodes[2 * i + 1]->val;
            auto mn = std::make_shared<MerkleNode>(mHashFunc(concat), i);
            levelNodes[2 * i]->parent = mn;
            if (hasRightNeighbor) {
                levelNodes[2 * i + 1]->parent = mn;
                levelNodes[2 * i + 1]->sibling = levelNodes[2 * i];
                levelNodes[2 * i]->sibling = levelNodes[2 * i + 1];
            }
            levelNodes[i] = mn;
        }
        numNodesInLevel = numNodesNextLevel;
    }
}

string MerkleTree::getRoot() const {
    std::shared_ptr<MerkleNode> curr = mMerkleDataByKey.begin()->second;
    while (curr->parent) curr = curr->parent;
    return curr->val;
}

VO MerkleTree::getVO(const string &k) const {
    auto md = findByKey(k);
    VO verificationObj;
    computeVOForMerkleData(md, verificationObj);
    return verificationObj;
}

void MerkleTree::computeVOForMerkleData(std::shared_ptr<const MerkleData> md,
                                        VO &verificationObj) const {
    verificationObj.val = md->val;
    std::shared_ptr<MerkleNode> currMn = md->parent;
    while (currMn->parent != NULL) {
        verificationObj.mSiblingPath.push_back(currMn->sibling.lock()->val);
        currMn = currMn->parent;
    }
}

std::shared_ptr<MerkleData> MerkleTree::findByKey(const string &k) const {
    auto mdIt = mMerkleDataByKey.find(k);
    if (mdIt == mMerkleDataByKey.end()) {
        throw std::invalid_argument(k + " does not exist in the MerkleTree.");
    }
    return mdIt->second;
}

void MerkleTree::update(string k, string v) {
    auto mdIt = mMerkleDataByKey.find(k);
    if (mdIt == mMerkleDataByKey.end()) {
        throw std::invalid_argument("Invalid key: " + k);
    }
    auto md = mdIt->second;
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
