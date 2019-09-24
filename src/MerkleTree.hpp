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
    MerkleNode* parent;
    MerkleNode* sibling;
    MerkleNode(string val, int relI)
        : val(val), mRelI(relI), parent(NULL), sibling(NULL) {}
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
    ~MerkleTree();
    string getRoot() const;
    VO getVO(const string& k) const;
    void update(string k, string v);

   private:
    string root;
    map<string, MerkleData*> kmd_map;
    const function<string(string)> mHashFunc;
    void computeVOForMerkleData(const MerkleData* const md,
                                VO& verif_obj) const;
    MerkleData* findByKey(const string& k) const;
};
