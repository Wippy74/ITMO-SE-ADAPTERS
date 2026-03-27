#pragma once

#include "traits.h"

template <template <typename, typename...> typename Container, typename... Args>
class SplitExpectedAdapter {
public:
  template <typename DataFlow>
  auto Apply(DataFlow& data_flow) {
    auto error_stream = data_flow.template Make<typename DataFlow::Type::error_type, Container, Args...>(
                                                [](auto& input, auto& error_output) {
                                                  for (auto& result : input) {
                                                    if (!result) {
                                                      error_output.push_back(result.error());
                                                    }
                                                  }
                                                });
        
    auto value_stream = data_flow.template Make<typename DataFlow::Type::value_type, Container, Args...>(
                                                [](auto& input, auto& value_output) {
                                                  for (auto& result : input) {
                                                    if (result) {
                                                      value_output.push_back(result.value());
                                                    }
                                                  }
                                                });
    return std::pair{error_stream, value_stream};
  }
};

template <template <typename, typename...> typename Container = SameCont, typename... Args>
auto SplitExpected() {
  return SplitExpectedAdapter<Container, Args...>();
}