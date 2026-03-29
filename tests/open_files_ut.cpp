#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

class OpenFilesTest : public ::testing::Test {
protected:
  std::filesystem::path main_dir;
  
  void SetUp() override {
    main_dir = std::filesystem::temp_directory_path() / "open_files_test";
    std::filesystem::create_directories(main_dir);
    
    std::ofstream(main_dir / "file1.txt") << "labwork";
    std::ofstream(main_dir / "file2.txt") << "eight";
  }
  
  void TearDown() override {
    std::filesystem::remove_all(main_dir);
  }
};

TEST_F(OpenFilesTest, readFile) {
  auto result = Dir(main_dir, false)
      | OpenFiles()
      | Transform([](std::ifstream& f) {
          std::string content;
          std::getline(f, content);
          return content;
      })
      | AsVector();
  
  ASSERT_EQ(result.size(), 2);
  ASSERT_THAT(result, testing::UnorderedElementsAre("labwork", "eight"));
}

TEST_F(OpenFilesTest, CombineWithSplit) {
  std::ofstream(main_dir / "labwork8.txt") << "dream,believe,makeithappen";
  
  std::vector<std::filesystem::path> paths = {main_dir / "labwork8.txt"};
  
  auto result = AsDataFlow(paths) | OpenFiles() | Split(",") | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("dream", "believe", "makeithappen"));
}