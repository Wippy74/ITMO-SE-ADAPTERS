#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>


class DirTest : public ::testing::Test {
protected:
  std::filesystem::path main_dir;
  
  void SetUp() override {
    main_dir = std::filesystem::temp_directory_path() / "main_dir";
    std::filesystem::create_directories(main_dir);
    std::filesystem::create_directories(main_dir / "sub_dir");
    
    std::ofstream(main_dir / "file1.txt") << "smth1";
    std::ofstream(main_dir / "file2.txt") << "smth2";
    std::ofstream(main_dir / "sub_dir" / "file67.txt") << "smth3";
  }
  
  void TearDown() override {
    std::filesystem::remove_all(main_dir);
  }
};

TEST_F(DirTest, nonRecursive) {
    auto result = Dir(main_dir, false) | Transform([](std::filesystem::path p) { return p.filename().string(); }) | AsVector();
    
    ASSERT_EQ(result.size(), 3u);
    ASSERT_THAT(result, testing::UnorderedElementsAre("file1.txt", "file2.txt", "sub_dir"));
}

TEST_F(DirTest, recursive) {
  auto result = Dir(main_dir, true) | Transform([](std::filesystem::path p) { return p.filename().string(); }) | AsVector();
  
  ASSERT_EQ(result.size(), 4u);
  ASSERT_THAT(result, testing::UnorderedElementsAre("file1.txt", "file2.txt", "file67.txt", "sub_dir"));
}

TEST_F(DirTest, filterByExtension) {
  std::ofstream(main_dir / "labwork100.cpp") << "cpp";
  
  auto result = Dir(main_dir, false) | Filter([](const std::filesystem::path& p) { return p.extension() == ".cpp"; }) | AsVector();
  
  ASSERT_EQ(result.size(), 1);
  ASSERT_THAT(result, testing::UnorderedElementsAre("/tmp/main_dir/labwork100.cpp"));
}