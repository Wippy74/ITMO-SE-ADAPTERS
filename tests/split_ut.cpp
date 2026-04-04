#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sstream>
#include <vector>

TEST(ReadTest, ByNewLine) {
    std::vector<std::stringstream> files(2);
    files[0] << "1\n2\n3\n4\n5";
    files[1] << "6\n7\n8\n9\n10";
    auto result = AsDataFlow(files) | Split("\n") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}

TEST(ReadTest, BySpace) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ") | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}


TEST(SplitTest, byComma) {
  std::vector<std::string> input = {"abc,bcd,cde"};
  
  auto result = AsDataFlow(input) | Split(",")  | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("abc", "bcd", "cde"));
}

TEST(SplitTest, byLetter) {
  std::vector<std::string> input = {"abc,bcd,cde"};
  
  auto result = AsDataFlow(input) | Split("b") | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("a", "c,", "cd,cde"));
}

TEST(SplitTest, multipleDelimiters) {
  std::vector<std::string> input = {"ABC!DE F.HIJK;LMN,O"};
  
  auto result = AsDataFlow(input) | Split(" .!;,") | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("ABC", "DE", "F", "HIJK", "LMN", "O"));
}

TEST(SplitTest, emptyTokens) {
  std::vector<std::string> input = {"1||2"};
  
  auto result = AsDataFlow(input) | Split("|") | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("1", "", "2"));
}

TEST(SplitTest, stringStream) {
  std::vector<std::stringstream> streams(1);
  streams[0] << "lab,work,eight";

  auto result = AsDataFlow(streams) | Split(",") | AsVector();

  ASSERT_THAT(result, testing::ElementsAre("lab", "work", "eight"));
}

TEST(SplitTest, delimiterAtStart) {
  std::vector<std::string> input = {",abc,def"};
  auto result = AsDataFlow(input) | Split(",") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("", "abc", "def"));
}

TEST(SplitTest, delimiterAtEnd) {
  std::vector<std::string> input = {"abc,def,"};
  auto result = AsDataFlow(input) | Split(",") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("abc", "def", ""));
}

TEST(SplitTest, noDelimiterInString) {
  std::vector<std::string> input = {"hello"};
  auto result = AsDataFlow(input) | Split(",") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("hello"));
}

TEST(SplitTest, emptyString) {
  std::vector<std::string> input = {""};
  auto result = AsDataFlow(input) | Split(",") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(""));
}

TEST(SplitTest, multipleStringsAccumulate) {
  std::vector<std::string> input = {"a,b", "c,d", "e"};
  auto result = AsDataFlow(input) | Split(",") | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("a", "b", "c", "d", "e"));
}
