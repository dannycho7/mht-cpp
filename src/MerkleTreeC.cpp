#define CAST_MHT(x) (static_cast<MerkleTree*>(x))

#include <stdexcept>
#include "MerkleTree.hpp"

extern "C" {
/*
 * @param[in] raw_tuples a char* array where tuples occupy 2 contiguous spots
 * @param[in] rt_size
 * @param[out] MerkleTree instance
 */
void* mht_create(const char* raw_tuples[], int rt_size,
                 const char* (*hashFunc)(const char*)) {
    // convert to map<string, string>
    map<string, string> raw_data;
    for (int i = 0; i < rt_size; i += 2) {
        if (raw_data.find(raw_tuples[i]) != raw_data.end()) {
            throw std::invalid_argument("Cannot have duplicate keys.");
        }
        raw_data[raw_tuples[i]] = raw_tuples[i + 1];
    }
    return new MerkleTree(raw_data, [hashFunc](const string& in) {
        return hashFunc(in.c_str());
    });
}

/*
 * @param[in] mht a MerkleTree ptr
 */
void mht_destroy(void* mht) { delete CAST_MHT(mht); }

/*
 * @param[in] mht a MerkleTree ptr
 * @param[out] root of the mht
 */
char* mht_get_root(void* mht) {
    std::string root_str = CAST_MHT(mht)->getRoot();
    char* root = new char[root_str.length() + 1];
    std::strcpy(root, root_str.c_str());
    return root;
}

struct VO_C {
    char* val;
    char** sibling_path;
    int sibling_size;
};

/*
 * @param[in] mht a MerkleTree ptr
 * @param[in] k the key you want the VO for
 * @param[out] VO_C instance
 */
VO_C mht_get_vo(void* mht, const char* k) {
    auto mht_cast = CAST_MHT(mht);
    VO vo_cpp = mht_cast->getVO(k);
    char* val = new char[vo_cpp.val.length() + 1];
    std::strcpy(val, vo_cpp.val.c_str());

    int size = vo_cpp.sibling_path.size();
    std::vector<char*> buf(size);
    auto s_path = vo_cpp.sibling_path;
    std::transform(s_path.begin(), s_path.end(), buf.begin(),
                   [](const std::string& arg) {
                       char* s_val = new char[arg.length() + 1];
                       std::strcpy(s_val, arg.c_str());
                       return s_val;
                   });
    char** sibling_path_c_arr = new char*[size];
    std::copy(buf.begin(), buf.end(), sibling_path_c_arr);

    return VO_C{val, sibling_path_c_arr, size};
}

/*
 * @param[in] mht a MerkleTree ptr
 * @param[in] k the key you want to modify the value for
 * @param[in] v the new value
 */
void mht_update(void* mht, const char* k, const char* v) {
    CAST_MHT(mht)->update(k, v);
}
}