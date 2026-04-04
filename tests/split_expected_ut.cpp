#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <expected>
#include <sstream>
#include <string>
#include <vector>

struct Department {
    std::string name;

    bool operator==(const Department& other) const = default;
};

std::expected<Department, std::string> ParseDepartment(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Department name is empty");
    }
    if (str.contains(' ')) {
        return std::unexpected("Department name contains space");
    }
    return Department{str};
}

TEST(SplitExpectedTest, SplitExpected) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|bad department||another-good-department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(ParseDepartment) | SplitExpected();

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name is empty.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"good-department"}, Department{"another-good-department"}));
}

TEST(SplitExpectedTest, allSuccess) {
  std::vector<std::pair<int, int>> input = {{102, 2}, {120, 4}};
  
  auto [errors, values] = AsDataFlow(input) 
      | Transform([](std::pair<int, int> p) -> std::expected<int, std::string> {
          if (p.second == 0) {
            return std::unexpected("Division by zero");
          }
          return p.first / p.second;
      })
      | SplitExpected();
  
  auto error_result = errors | AsVector();
  auto value_result = values | AsVector();
  
  ASSERT_TRUE(error_result.empty());
  ASSERT_THAT(value_result, testing::ElementsAre(51, 30));
}


TEST(SplitExpectedTest, allErrors) {
  std::vector<std::pair<int, int>> input = {{102, 0}, {120, 0}};

  auto [errors, values] = AsDataFlow(input)
      | Transform([](std::pair<int, int> p) -> std::expected<int, std::string> {
          if (p.second == 0) {
            return std::unexpected("Division by zero");
          }
          return p.first / p.second;
      })
      | SplitExpected();

  auto error_result = errors | AsVector();
  auto value_result = values | AsVector();

  ASSERT_THAT(error_result, testing::Each("Division by zero"));
  ASSERT_TRUE(value_result.empty());
}

TEST(SplitExpectedTest, mixedResults) {
  std::vector<int> input = {-2, -1, 0, 1, 2};

  auto [errors, values] = AsDataFlow(input)
      | Transform([](int x) -> std::expected<int, std::string> {
          if (x < 0) return std::unexpected("negative");
          return x * 10;
      })
      | SplitExpected();

  auto error_result = errors | AsVector();
  auto value_result = values | AsVector();

  ASSERT_THAT(error_result, testing::ElementsAre("negative", "negative"));
  ASSERT_THAT(value_result, testing::ElementsAre(0, 10, 20));
}

TEST(SplitExpectedTest, emptyInput) {
  std::vector<int> input;

  auto [errors, values] = AsDataFlow(input)
      | Transform([](int x) -> std::expected<int, std::string> { return x; })
      | SplitExpected();

  ASSERT_TRUE((errors | AsVector()).empty());
  ASSERT_TRUE((values | AsVector()).empty());
}

TEST(SplitExpectedTest, singleSuccess) {
  std::vector<std::string> input = {"valid"};

  auto [errors, values] = AsDataFlow(input)
      | Transform(ParseDepartment)
      | SplitExpected();

  auto value_result = values | AsVector();
  auto error_result = errors | AsVector();

  ASSERT_TRUE(error_result.empty());
  ASSERT_THAT(value_result, testing::ElementsAre(Department{"valid"}));
}