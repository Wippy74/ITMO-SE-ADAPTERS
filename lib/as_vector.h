#pragma once

#include <vector>

class AsVectorAdapter {
public:
  template <typename Data>
  auto Apply(Data& data) const {
    using ValueType = typename Data::Type;
    std::vector<ValueType> result;
    for (auto& item : data.Access()) {
      result.push_back(item);
    }
    return result;
  }
};

inline auto AsVector() {
  return AsVectorAdapter{};
}