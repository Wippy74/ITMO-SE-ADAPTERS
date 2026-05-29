#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>
#include <ranges>
#include <string>
#include <vector>

TEST(DropNulloptTest, DropNullopt) {
    std::vector<std::optional<int>> input = {1, std::nullopt, 3, std::nullopt, 5};
    auto result = AsDataFlow(input) | DropNullopt() | AsVector();
    ASSERT_THAT(result, testing::ElementsAre(1, 3, 5));
}

TEST(DropNulloptTest, allNullopt) {
  std::vector<std::optional<int>> input = {std::nullopt, std::nullopt, std::nullopt};
  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_TRUE(result.empty());
}

TEST(DropNulloptTest, noNullopt) {
  std::vector<std::optional<int>> input = {10, 20, 30};
  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(10, 20, 30));
}

TEST(DropNulloptTest, emptyInput) {
  std::vector<std::optional<int>> input;
  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_TRUE(result.empty());
}

TEST(DropNulloptTest, dropNulloptStrings) {
  std::vector<std::optional<std::string>> input = {"hello", std::nullopt, "world"};
  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_THAT(result, testing::ElementsAre("hello", "world"));
}

TEST(DropNulloptTest, combinedWithTransform) {
  std::vector<int> raw = {-1, 2, -3, 4, 5};
  auto optional_input = raw
    | std::views::transform([](int x) -> std::optional<int> {
        return x > 0 ? std::optional<int>{x} : std::nullopt;
    });
  std::vector<std::optional<int>> input(optional_input.begin(), optional_input.end());

  auto result = AsDataFlow(input) | DropNullopt() | AsVector();
  ASSERT_THAT(result, testing::ElementsAre(2, 4, 5));
}
