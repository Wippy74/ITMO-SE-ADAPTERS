#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>


class DirTest : public ::testing::Test {
protected:
  std::filesystem::path test_dir_;
  
  void SetUp() override {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(10000, 99999);
    
    test_dir_ = std::filesystem::temp_directory_path() / 
                ("dir_test_" + std::to_string(dis(gen)));
    std::error_code ec;
    std::filesystem::remove_all(test_dir_, ec);
    std::filesystem::create_directories(test_dir_);
  }
    
  void TearDown() override {
    std::error_code ec;
    std::filesystem::remove_all(test_dir_, ec);
  }
    
  void CreateFile(const std::filesystem::path& relative_path, const std::string& content = "test") {
    auto full_path = test_dir_ / relative_path;

    if (full_path.has_parent_path()) {
      std::filesystem::create_directories(full_path.parent_path());
    }
    
    std::ofstream file(full_path);
    file << content;
  }
};

TEST_F(DirTest, nonRecursive) {
  CreateFile("file1.txt");
  CreateFile("file2.txt");
  CreateFile("sub_dir/file67.txt");
  auto result = Dir(test_dir_, false) | Transform([](std::filesystem::path p) { return p.filename().string(); }) | AsVector();
  
  ASSERT_EQ(result.size(), 3u);
  ASSERT_THAT(result, testing::UnorderedElementsAre("file1.txt", "file2.txt", "sub_dir"));
}

TEST_F(DirTest, recursive) {
  CreateFile("file1.txt");
  CreateFile("file2.txt");
  CreateFile("sub_dir/file67.txt");
  auto result = Dir(test_dir_, true) | Transform([](std::filesystem::path p) { return p.filename().string(); }) | AsVector();
  
  ASSERT_EQ(result.size(), 4u);
  ASSERT_THAT(result, testing::UnorderedElementsAre("file1.txt", "file2.txt", "file67.txt", "sub_dir"));
}

TEST_F(DirTest, filterByExtension) {
  CreateFile("file1.txt");
  CreateFile("file2.txt");
  CreateFile("labwork100.cpp");
  
  auto result = Dir(test_dir_, false) | Filter([](const std::filesystem::path& p) { return p.extension() == ".cpp"; }) 
      | Transform([](const std::filesystem::path& p) { return p.filename().string(); }) | AsVector();
  
  ASSERT_EQ(result.size(), 1);
  ASSERT_THAT(result, testing::UnorderedElementsAre("labwork100.cpp"));
}