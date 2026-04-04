#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

TEST(FilterTest, emptyResult) {
  std::vector<int> input = {11, 23, 35, 76};

  auto result = AsDataFlow(input) | Filter([](int x) { return x % 67 == 25; }) | AsVector();

  ASSERT_TRUE(result.empty());
}

TEST(FilterTest, FilterEven) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(FilterTest, FilterUpperCase) {
    std::vector<std::string> input = {"hello", "world", "HELLO", "WORLD"};
    auto result =
        AsDataFlow(input)
            | Filter([](const std::string& x) { return std::all_of(x.begin(), x.end(), [](char c) { return std::isupper(c); }); })
            | AsVector();
    ASSERT_THAT(result, testing::ElementsAre("HELLO", "WORLD"));
}

TEST(FilterTest, allPass) {
  std::vector<int> input = {2, 4, 6, 8};
  auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(2, 4, 6, 8));
}

TEST(FilterTest, emptyInput) {
  std::vector<int> input;
  auto result = AsDataFlow(input) | Filter([](int x) { return x > 0; }) | AsVector();
  ASSERT_TRUE(result.empty());
}

TEST(FilterTest, filterByStringLength) {
  std::vector<std::string> input = {"a", "bb", "ccc", "dddd", "eeeee"};
  auto result = AsDataFlow(input) | Filter([](const std::string& s) { return s.size() >= 3; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("ccc", "dddd", "eeeee"));
}

TEST(FilterTest, chainedFilters) {
  std::vector<int> input = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  auto result = AsDataFlow(input)
    | Filter([](int x) { return x % 2 == 0; })
    | Filter([](int x) { return x > 4; })
    | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(6, 8, 10));
}

TEST(FilterTest, filterNegatives) {
  std::vector<int> input = {-3, -1, 0, 1, 3, 5};
  auto result = AsDataFlow(input) | Filter([](int x) { return x >= 0; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(0, 1, 3, 5));
}
