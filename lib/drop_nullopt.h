#pragma once

#include "traits.h"

template <template <typename, typename...> typename C, typename... Args>
class DropNulloptAdapter {
public:
  template <typename Data>
  auto Apply(Data& data) const {
    return data.template Make<typename Data::Type::value_type, C, Args...>([](auto& input_container, auto& output_container) {
                                                                              for (const auto& v : input_container) {
                                                                                if (v.has_value()) {
                                                                                  output_container.push_back(v.value());
                                                                                }
                                                                              }
                                                                            });
  }
};

template <template <typename, typename...> typename C = SameCont, typename... Args>
auto DropNullopt() {
  return DropNulloptAdapter<C, Args...>();
}