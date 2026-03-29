#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(AsVectorTest, intVector) {
  std::vector<int> input = {1, 2, 3};
  
  auto result = AsDataFlow(input) | Transform([](int x) { return x * 67; }) | AsVector();
  
  ASSERT_EQ(result.size(), 3);
  ASSERT_THAT(result, testing::ElementsAre(67, 134, 201));
}

TEST(AsVectorTest, emptyInput) {
  std::vector<int> input;
  
  auto result = AsDataFlow(input) | AsVector();
  
  ASSERT_TRUE(result.empty());
}

TEST(AsVectorTest, typeChange) {
  std::vector<int> input = {11, 22, 33};
  
  auto result = AsDataFlow(input) | Transform([](int x) { return std::to_string(x); }) | AsVector();
  
  static_assert(std::is_same_v<decltype(result), std::vector<std::string>>);
  ASSERT_THAT(result, testing::ElementsAre("11", "22", "33"));
}