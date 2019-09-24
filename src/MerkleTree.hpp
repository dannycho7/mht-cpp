#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

using std::function;
using std::map;
using std::string;
using std::vector;

struct MerkleNode {
    string val;
    int mRelI;  // relative index: used to determine which sibling goes first
    std::shared_ptr<MerkleNode> parent;
    std::weak_ptr<MerkleNode> sibling;
    MerkleNode(string val, int relI) : val(val), mRelI(relI) {}
};

struct MerkleData : MerkleNode {
    MerkleData(string val, int relI) : MerkleNode(val, relI) {}
};

struct VO {
    string val;
    vector<string> sibling_path;
};

class MerkleTree {
   public:
    MerkleTree(const map<string, string>& rawData,
               function<string(string)> hashFunc);
    ~MerkleTree() = default;
    string getRoot() const;
    VO getVO(const string& k) const;
    void update(string k, string v);

   private:
    string root;
    map<string, std::shared_ptr<MerkleData>> kmd_map;
    const function<string(string)> mHashFunc;
    void computeVOForMerkleData(std::shared_ptr<const MerkleData> md,
                                VO& verif_obj) const;
    std::shared_ptr<MerkleData> findByKey(const string& k) const;
};
