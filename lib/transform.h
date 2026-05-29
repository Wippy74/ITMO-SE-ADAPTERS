#pragma once

#include "traits.h"

#include <algorithm>
#include <iterator>

template <typename Function, template <typename, typename...> typename C, typename... Args>
class TransformAdapter {
private:
  Function transform_func_; 
public:
  TransformAdapter(const Function& func) : transform_func_(func) {}

  template <typename InputType>
  using ResultType = decltype(transform_func_(std::declval<InputType&>()));

  template <typename Data>
  auto Apply(Data& data) const {
    const Function& func = transform_func_;
    return data.template Make<ResultType<typename Data::Type>, C, Args...>([&func](auto& input_container, auto& output_container) {
                                                                            std::transform(input_container.begin(), input_container.end(),
                                                                                            std::back_inserter(output_container),
                                                                                            func);
                                                                            });
  }
};

template <template <typename, typename...> typename C = SameCont, typename... Args, typename Function>
auto Transform(Function func) {
  return TransformAdapter<Function, C, Args...>(func);
}