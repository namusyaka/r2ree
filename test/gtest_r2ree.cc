#include "gtest/gtest.h"
#include "r2ree.hh"

namespace {
  class r2ree_test : public ::testing::Test{
    protected:
      r2ree::radix_tree tree;
  };

  TEST_F(r2ree_test, insert_basic) {
    tree.insert("/user");
    ASSERT_EQ(0, tree.cid);
  }

  TEST_F(r2ree_test, insert_with_multiple_routes) {
    tree.insert("/user");
    tree.insert("/123");
    ASSERT_EQ(1, tree.cid);
  }

  TEST_F(r2ree_test, insert_with_no_route) {
    ASSERT_EQ(-1, tree.cid);
  }

  TEST_F(r2ree_test, get_basic) {
    r2ree::parse_result actual;

    tree.insert("/user");
    actual = tree.get("/user");
    ASSERT_EQ(true, std::get<0>(actual));
    ASSERT_EQ(0,    std::get<1>(actual));
    ASSERT_EQ(0,    std::get<2>(actual).size);
    ASSERT_EQ(true, std::get<3>(actual));
  }

  TEST_F(r2ree_test, get_with_nonexistence_pattern) {
    r2ree::parse_result actual;

    tree.insert("/user");
    actual = tree.get("/users");
    ASSERT_EQ(false, std::get<0>(actual));
    ASSERT_EQ(0,     std::get<1>(actual));
    ASSERT_EQ(0,     std::get<2>(actual).size);
    ASSERT_EQ(false, std::get<3>(actual));
  }
}
