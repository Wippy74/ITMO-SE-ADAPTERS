#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

struct Employee {
    uint64_t department_id;
    std::string name;

    bool operator==(const Employee& other) const = default;
};

TEST(AggregateByKeyTest, emptyInput) {
    std::vector<std::string> input;
    
    auto result = AsDataFlow(input) 
        | AggregateByKey(
            size_t{0},
            [](std::string&, size_t& count) { ++count; },
            [](std::string& word) { return word; }
        )
        | AsVector();
    
    ASSERT_TRUE(result.empty());
}

TEST(AggregateByKeyTest, CountingAggregatedValues) {
    std::vector<std::string> input = {"name4", "name0", "name1", "name0", "name2", "name0", "name1"};

    auto result =
        AsDataFlow(input)
            | AggregateByKey(
                std::size_t{0},
                [](const std::string&, std::size_t& accumulated) { ++accumulated; },
                [](const std::string& token) { return token; }
            )
            | AsVector();

    ASSERT_THAT(
        result,
        ::testing::ElementsAre(
            std::make_pair("name4", 1),
            std::make_pair("name0", 3),
            std::make_pair("name1", 2),
            std::make_pair("name2", 1)
        )
    );
}

TEST(AggregateByKeyTest, sumByKey) {
  std::vector<std::pair<std::string, int>> input = {
    {"a", 1}, {"b", 10}, {"a", 2}, {"b", 20}, {"a", 3}
  };

  auto result = AsDataFlow(input)
    | AggregateByKey(
        int{0},
        [](const std::pair<std::string, int>& p, int& sum) { sum += p.second; },
        [](const std::pair<std::string, int>& p) { return p.first; }
    )
    | AsVector();

  ASSERT_THAT(result, testing::UnorderedElementsAre(
    std::make_pair(std::string("a"), 6),
    std::make_pair(std::string("b"), 30)
  ));
}

TEST(AggregateByKeyTest, singleElementPerKey) {
  std::vector<int> input = {3, 1, 4, 1, 5, 9, 2, 6};

  auto result = AsDataFlow(input)
    | AggregateByKey(
        int{0},
        [](const int& v, int& sum) { sum += v; },
        [](const int& v) { return v % 3; }
    )
    | AsVector();

  ASSERT_EQ(result.size(), 3u);
}

TEST(AggregateByKeyTest, AggregatingWithSeveralOutputsForEachKey) {
    std::vector<Employee> employees = {
        {3, "name1"},
        {1, "name1"},
        {1, "name2"},
        {2, "name1"},
        {2, "name2"},
        {2, "name3"}
    };

    auto result =
        AsDataFlow(employees)
            | AggregateByKey(
                std::vector<Employee>{},
                [](const Employee& employee, std::vector<Employee>& accumulated) {
                    if (accumulated.size() == 2) {
                        return;
                    }
                    accumulated.push_back(employee);
                },
                [](const Employee& employee) { return employee.department_id; }
            )
            | AsVector();

    ASSERT_THAT(
        result,
        ::testing::ElementsAre(
            std::make_pair(3, std::vector<Employee>{Employee{3, "name1"}}),
            std::make_pair(1, std::vector<Employee>{Employee{1, "name1"}, Employee{1, "name2"}}),
            std::make_pair(2, std::vector<Employee>{Employee{2, "name1"}, Employee{2, "name2"}})
        )
    );
}
