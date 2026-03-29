#include <processing.h>

#include <gtest/gtest.h>

#include <sstream>
#include <vector>

TEST(WriteTest, Write) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    std::stringstream file_emulator;
    auto result = AsDataFlow(input) | Write(file_emulator, '|');
    ASSERT_EQ(file_emulator.str(), "1|2|3|4|5|");
}

TEST(WriteTest, withDelimiter) {
    std::vector<int> input = {10, 29, 16};
    std::ostringstream oss;
    
    AsDataFlow(input) | Write(oss, "|");
    
    ASSERT_EQ(oss.str(), "10|29|16|");
}

TEST(WriteTest, emptyDelimiter) {
  std::vector<std::string> input = {"one", "two", "three"};
  std::ostringstream oss;
  
  AsDataFlow(input) | Write(oss, "");
  
  ASSERT_EQ(oss.str(), "onetwothree");
}

TEST(WriteTest, SingleElement) {
  std::vector<int> input = {80};
  std::ostringstream oss;
  
  AsDataFlow(input) | Write(oss, ", ");
  
  ASSERT_EQ(oss.str(), "80, ");
}
