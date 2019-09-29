#define CAST_MHT(mht) static_cast<MerkleTree*>(mht)

#include <stdexcept>
#include "MerkleTree.hpp"

extern "C" {
/*
 * @param[in] rawTuples a char* array where tuples occupy 2 contiguous spots
 * @param[in] rtSize
 * @param[out] MerkleTree instance
 */
void* mhtCreate(const char* rawTuples[], int rtSize,
                 const char* (*hashFunc)(const char*)) {
    map<string, string> rawData;
    for (int i = 0; i < rtSize; i += 2) {
        if (rawData.find(rawTuples[i]) != rawData.end()) {
            throw std::invalid_argument("Cannot have duplicate keys.");
        }
        rawData[rawTuples[i]] = rawTuples[i + 1];
    }
    return new MerkleTree(rawData, [hashFunc](const string& in) {
        return hashFunc(in.c_str());
    });
}

/*
 * @param[in] mht a MerkleTree ptr
 */
void mhtDestroy(void* mht) { delete CAST_MHT(mht); }

/*
 * @param[in] mht a MerkleTree ptr
 * @param[out] root of the mht
 */
char* mhtGetRoot(void* mht) {
    std::string rootStr = CAST_MHT(mht)->getRoot();
    char* root = new char[rootStr.length() + 1];
    std::strcpy(root, rootStr.c_str());
    return root;
}

struct VO_C {
    char* val;
    char** mSiblingPath;
    int mSiblingSize;
};

/*
 * @param[in] mht a MerkleTree ptr
 * @param[in] k the key you want the VO for
 * @param[out] VO_C instance
 */
VO_C mhtGetVO(void* mht, const char* k) {
    VO verificationObjCpp = CAST_MHT(mht)->getVO(k);
    char* val = new char[verificationObjCpp.val.length() + 1];
    std::strcpy(val, verificationObjCpp.val.c_str());

    int size = verificationObjCpp.mSiblingPath.size();
    std::vector<char*> buf(size);
    auto siblingPath = verificationObjCpp.mSiblingPath;
    std::transform(siblingPath.begin(), siblingPath.end(), buf.begin(),
                   [](const std::string& arg) {
                       char* val = new char[arg.length() + 1];
                       std::strcpy(val, arg.c_str());
                       return val;
                   });
    char** siblingPathCArr = new char*[size];
    std::copy(buf.begin(), buf.end(), siblingPathCArr);

    return VO_C{val, siblingPathCArr, size};
}

/*
 * @param[in] mht a MerkleTree ptr
 * @param[in] k the key you want to modify the value for
 * @param[in] v the new value
 */
void mhtUpdate(void* mht, const char* k, const char* v) {
    CAST_MHT(mht)->update(k, v);
}
}