#pragma once

#include <vector>

class AsVectorAdapt {
  template <typename T, typename ... Args>
  auto operator()() const {
    return std::vector<T, Args ...>{};
  };

  template <typename Data>
  auto operator()(Data& data) const {
    return std::vector<typename std::decay_t<decltype(*data.begin())>{data.begin(), data.end()};
  };
};

constexpr AsVectorAdapt AsVector;