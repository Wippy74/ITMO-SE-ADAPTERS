#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <sstream>
#include <vector>

TEST(TransformTest, PowerOfTwo) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 4, 9, 16, 25));
}

TEST(TransformTest, FromStringToInt) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ") | Transform([](const std::string& str) { return std::stoi(str); }) | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10));
}

TEST(TransformTest, intToString) {
  std::vector<int> input = {10, 20, 30};
  
  auto result = AsDataFlow(input) | Transform([](int x) { return std::to_string(x) + "a"; }) | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("10a", "20a", "30a"));
}

TEST(TransformTest, compositionOfTransforms) {
  std::vector<int> input = {4, 5, 6};

  auto result = AsDataFlow(input) | Transform([](int x) { return x + 2; }) | Transform([](int x) { return x * 5; })| AsVector();

  ASSERT_THAT(result, testing::ElementsAre(30, 35, 40));
}

TEST(TransformTest, emptyInput) {
  std::vector<int> input;
  auto result = AsDataFlow(input) | Transform([](int x) { return x * 2; }) | AsVector();
  ASSERT_TRUE(result.empty());
}

TEST(TransformTest, identityTransform) {
  std::vector<std::string> input = {"foo", "bar", "baz"};
  auto result = AsDataFlow(input) | Transform([](const std::string& s) { return s; }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("foo", "bar", "baz"));
}

TEST(TransformTest, pairConstruction) {
  std::vector<int> input = {1, 2, 3};
  auto result = AsDataFlow(input) | Transform([](int x) { return std::make_pair(x, x * x); }) | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(
    std::make_pair(1, 1),
    std::make_pair(2, 4),
    std::make_pair(3, 9)
  ));
}

TEST(TransformTest, transformAfterFilter) {
  std::vector<int> input = {1, 2, 3, 4, 5, 6};
  auto result = AsDataFlow(input)
    | Filter([](int x) { return x % 2 == 0; })
    | Transform([](int x) { return x * 10; })
    | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(20, 40, 60));
}