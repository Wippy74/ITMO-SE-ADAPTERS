#include "processing.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

class OpenFilesTest : public ::testing::Test {
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

TEST_F(OpenFilesTest, readFile) {
  CreateFile("file1.txt", "labwork");
  CreateFile("file2.txt", "eight");
  auto result = Dir(test_dir_, false)
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
  CreateFile("labwork8.txt", "dream,believe,makeithappen");
  
  std::vector<std::filesystem::path> paths = {test_dir_ / "labwork8.txt"};
  
  auto result = AsDataFlow(paths) | OpenFiles() | Split(",") | AsVector();
  
  ASSERT_THAT(result, testing::ElementsAre("dream", "believe", "makeithappen"));
}