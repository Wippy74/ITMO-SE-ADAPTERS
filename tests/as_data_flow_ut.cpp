#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(AsDataFlowTest, nonConstVector) {
  std::vector<int> input = {2, 1, 3, 5, 4};
  
  auto result = AsDataFlow(input) | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre(2, 1, 3, 5, 4));
}

TEST(AsDataFlowTest, constVector) {
  const std::vector<int> input = {101, 220, 303};

  auto result = AsDataFlow(input) | AsVector();

  ASSERT_THAT(result, testing::ElementsAre(101, 220, 303));
}

TEST(AsDataFlowTest, emptyVector) {
  std::vector<int> input;
  auto result = AsDataFlow(input) | AsVector();
  ASSERT_TRUE(result.empty());
}

TEST(AsDataFlowTest, doesNotModifyOriginal) {
  std::vector<int> input = {1, 2, 3};
  auto flow = AsDataFlow(input);
  flow | Transform([](int x) { return x * 100; }) | AsVector();
  ASSERT_THAT(input, testing::ElementsAre(1, 2, 3));
}

TEST(AsDataFlowTest, stringVector) {
  std::vector<std::string> input = {"one", "two", "three"};
  auto result = AsDataFlow(input) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("one", "two", "three"));
}