#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

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