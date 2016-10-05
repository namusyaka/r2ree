#include "gtest/gtest.h"
#include "r2ree.hh"

namespace {
  class r2ree_test : public ::testing::Test{};

  r2ree::handle_func handle = [](r2ree::request *req) {
  };

  TEST_F(r2ree_test, insert_basic) {
    r2ree::radix_tree tree;
    tree.insert("/user", handle, {"GET"});
    EXPECT_EQ(0, tree.cid);
  }

  TEST_F(r2ree_test, insert_with_multiple_routes) {
    r2ree::radix_tree tree;
    tree.insert("/user", handle, {"GET"});
    tree.insert("/user", handle, {"POST"});
    EXPECT_EQ(1, tree.cid);
  }
}
