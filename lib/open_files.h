#pragma once

#include "traits.h"
#include "transform.h"

#include <filesystem>
#include <functional>
#include <fstream>

template <template <typename, typename...> typename Container, typename... Args>
class OpenFilesAdapter : public TransformAdapter<std::function<std::ifstream(std::filesystem::path)>, Container, Args...> {
public:
  OpenFilesAdapter() : TransformAdapter<std::function<std::ifstream(std::filesystem::path)>, Container, Args...>(
                                                                    [](std::filesystem::path path) {
                                                                      return std::ifstream(path);
                                                                    }) {}
};

template <template <typename, typename...> typename Container = SameCont, typename... Args>
auto OpenFiles() {
  return OpenFilesAdapter<Container, Args...>();
}