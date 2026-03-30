#pragma once

#include "traits.h"

template <template <typename, typename...> typename Container, typename... Args>
class SplitExpectedAdapter {
public:
  template <typename Data>
  auto Apply(Data& data) {
    using ExpectedType = typename Data::Type;
    using ValueType = typename ExpectedType::value_type;
    using ErrorType = typename ExpectedType::error_type;
    using GetterType = typename  std::function<typename Data::Container&()>;

    std::vector<ValueType> values;
    std::vector<ErrorType> errors;

    for (auto& result : data.Access()) {
      if (result.has_value()) {
        values.push_back(result.value());
      } else {
        errors.push_back(result.error());
      }
    }

    auto error_stream = LazyPipeline<ErrorType, std::vector>(std::move(errors));
    auto value_stream = LazyPipeline<ValueType, std::vector>(std::move(values));
    
    return std::pair{std::move(error_stream), std::move(value_stream)};
  }
};

template <template <typename, typename...> typename Container = SameCont, typename... Args>
auto SplitExpected() {
  return SplitExpectedAdapter<Container, Args...>();
}