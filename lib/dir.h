#pragma once

#include "lazy_pipeline.h"
#include "traits.h"

#include <filesystem>
#include <vector>

template <template <typename, typename...> typename Container = std::vector, typename... Args>
auto Dir(std::filesystem::path dir_path, bool is_recursive) {
  if (!std::filesystem::is_directory(dir_path)) {
    throw std::runtime_error("Wrong input path");
  }
  using Flow = LazyPipeline<std::filesystem::path, Container, Args...>;

  return Flow([is_recursive, dir_path](typename Flow::Container& output) {
                      if (is_recursive) {
                        output = typename Flow::Container(std::filesystem::recursive_directory_iterator(dir_path),
                                                          std::filesystem::recursive_directory_iterator());
                      } else {
                        output = typename Flow::Container(std::filesystem::directory_iterator(dir_path), 
                                                          std::filesystem::directory_iterator());
                      }
                  });
}