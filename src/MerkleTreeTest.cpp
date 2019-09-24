#include <gtest/gtest.h>
#include <stdexcept>

#include "./PicoSHA2/picosha2.h"
#include "MerkleTree.hpp"

string hash_256(const string& in) {
    string hash_hex_str;
    picosha2::hash256_hex_string(in, hash_hex_str);
    return hash_hex_str;
}

TEST(MerkleTreeTest, withNoData) {
    vector<Tuple> data;
    EXPECT_THROW(MerkleTree mht(data, hash_256), std::invalid_argument);
}

TEST(MerkleTreeTest, withInvalidData) {
    vector<Tuple> data{{"k", "v"}, {"k", "v"}};
    EXPECT_THROW(MerkleTree mht(data, hash_256), std::invalid_argument);
}

TEST(MerkleTreeTest, getRootBasic) {
    vector<Tuple> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    EXPECT_EQ(mht.getRoot(), hash_256(hash_256("v1") + hash_256("v2")));
}

TEST(MerkleTreeTest, getRootWithOddNItems) {
    vector<Tuple> data{{"k1", "v1"}, {"k2", "v2"}, {"k3", "v3"}};
    MerkleTree mht{data, hash_256};

    EXPECT_EQ(mht.getRoot(),
              hash_256(hash_256(hash_256("v1") + hash_256("v2")) +
                       hash_256(hash_256("v3"))));
}

TEST(MerkleTreeTest, getVO) {
    vector<Tuple> data{{"k1", "v1"}, {"k2", "v2"}};
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
    vector<Tuple> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    mht.update("k1", "v3");
    EXPECT_EQ(mht.getRoot(), hash_256(hash_256("v3") + hash_256("v2")));
    mht.update("k2", "v4");
    EXPECT_EQ(mht.getRoot(), hash_256(hash_256("v3") + hash_256("v4")));
}

TEST(MerkleTreeTest, updateWithInvalidKey) {
    vector<Tuple> data{{"k1", "v1"}, {"k2", "v2"}};
    MerkleTree mht{data, hash_256};
    EXPECT_THROW(mht.update("invalid_key", "garbage_value"),
                 std::invalid_argument);
}
