#pragma once

#include <algorithm>
#include <iterator>

#include "traits.h"

template <typename Predicate, template <typename, typename...> typename C, typename... Args>
class FilterAdapter {
public:
  FilterAdapter(const Predicate& func) : predicate_(func) {}

  template <typename Data>
  auto Apply(Data& data) const {
    const Predicate& func = predicate_;
    return data.template Make<typename Data::Type, C, Args...>([&func](auto& input_cont, auto& output_cont) {
                                                                      std::remove_copy_if(input_cont.begin(), input_cont.end(),
                                                                      std::back_inserter(output_cont),
                                                                      [&func](auto& value) { return !func(value); });
                                                                      });
  }
private:
  Predicate predicate_;
};

template <template <typename, typename...> typename C = SameCont, typename... Args, typename Predicate>
auto Filter(Predicate func) {
  return FilterAdapter<Predicate, C, Args...>(func);
}