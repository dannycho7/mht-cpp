#include "MerkleTree.hpp"
#include "PicoSHA2/picosha2.h"

#include <gtest/gtest.h>
#include <stdexcept>

string hash_256(const string& in) {
    string hash_hex_str;
    picosha2::hash256_hex_string(in, hash_hex_str);
    return hash_hex_str;
}

TEST(MerkleTreeTest, withNoData) {
    map<string, string> data;
    EXPECT_THROW(MerkleTree mht(data, hash_256), std::invalid_argument);
}

TEST(MerkleTreeTest, getRootBasic) {
    map<string, string> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    EXPECT_EQ(mht.getRoot(), hash_256(hash_256("v1") + hash_256("v2")));
}

TEST(MerkleTreeTest, getRootWithOddNItems) {
    map<string, string> data{{"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}};
    MerkleTree mht{data, hash_256};

    EXPECT_EQ(mht.getRoot(),
              hash_256(hash_256(hash_256("v1") + hash_256("v2")) +
                       hash_256(hash_256("v3"))));
}

TEST(MerkleTreeTest, getVO) {
    map<string, string> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    VO vo1 = mht.getVO("k1");
    EXPECT_EQ(vo1.val, "v1");
    EXPECT_EQ(vo1.sibling_path.size(), 1);
    EXPECT_EQ(vo1.sibling_path[0], hash_256("v2"));

    VO vo2 = mht.getVO("k2");
    EXPECT_EQ(vo2.val, "v2");
    EXPECT_EQ(vo2.sibling_path.size(), 1);
    EXPECT_EQ(vo2.sibling_path[0], hash_256("v1"));
}

TEST(MerkleTreeTest, updateWithValidKey) {
    map<string, string> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    mht.update("k1", "v3");
    EXPECT_EQ(mht.getRoot(), hash_256(hash_256("v3") + hash_256("v2")));
    mht.update("k2", "v4");
    EXPECT_EQ(mht.getRoot(), hash_256(hash_256("v3") + hash_256("v4")));
}

TEST(MerkleTreeTest, updateWithInvalidKey) {
    map<string, string> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    EXPECT_THROW(mht.update("invalid_key", "garbage_value"),
                 std::invalid_argument);
}
