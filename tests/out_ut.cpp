#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <string>
#include <vector>

TEST(OutTest, stringOutput) {
  std::vector<std::string> input = {"labwork", "eight"};
  std::ostringstream oss;
  
  AsDataFlow(input) | Out(oss);
  
  ASSERT_EQ(oss.str(), "labwork\neight\n");
}

TEST(OutTest, emptyInput) {
  std::vector<int> input;
  std::ostringstream oss;

  AsDataFlow(input) | Out(oss);

  ASSERT_EQ(oss.str(), "");
}

TEST(OutTest, intOutput) {
  std::vector<int> input = {1, 2, 3};
  std::ostringstream oss;

  AsDataFlow(input) | Out(oss);

  ASSERT_EQ(oss.str(), "1\n2\n3\n");
}

TEST(OutTest, singleElement) {
  std::vector<std::string> input = {"only"};
  std::ostringstream oss;

  AsDataFlow(input) | Out(oss);

  ASSERT_EQ(oss.str(), "only\n");
}

TEST(OutTest, returnsFlowForChaining) {
  std::vector<int> input = {10, 20, 30};
  std::ostringstream oss;

  auto result = AsDataFlow(input) | Out(oss) | AsVector();

  ASSERT_EQ(oss.str(), "10\n20\n30\n");
  ASSERT_THAT(result, testing::ElementsAre(10, 20, 30));
}