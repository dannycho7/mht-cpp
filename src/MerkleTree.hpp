#pragma once

#include <functional>
#include <map>
#include <string>
#include <utility> /* std::pair */
#include <vector>

using std::function;
using std::map;
using std::pair;
using std::string;
using std::vector;

struct MerkleNode {
    string val;
    MerkleNode* parent;
    MerkleNode* sibling;
    MerkleNode(string val) : val(val), parent(NULL), sibling(NULL) {}
};

struct MerkleData : MerkleNode {
    MerkleData(string val) : MerkleNode(val) {}
};

struct VO {
    string val;
    vector<string> sibling_path;
};

typedef pair<string, string> Tuple; /* k, v */
typedef pair<string, MerkleData*> KDPair;

class MerkleTree {
   public:
    MerkleTree(const vector<Tuple>& tuples, function<string(string)> hashFunc);
    ~MerkleTree();
    string getRoot() const;
    VO getVO(const string& k) const;
    void update(string k, string v);

   private:
    string root;
    map<string, MerkleData*> kd_map;
    vector<MerkleData*> data;
    const function<string(string)> mHashFunc;
    void computeVOForMerkleData(const MerkleData* const md,
                                VO& verif_obj) const;
    MerkleData* findByKey(const string& k) const;
};
