#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>


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