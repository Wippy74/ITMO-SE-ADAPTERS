#pragma once

#include "lazy_pipeline.h"
#include "traits.h"

#include <utility>

template <typename ValueType, template <typename, typename...> typename Container, typename... Args>
auto AsDataFlow(Container<ValueType, Args...>& container) {
  return LazyPipeline<ValueType, Container, Args...>(container);
}

template <typename ValueType, template <typename, typename...> typename Container, typename... Args>
auto AsDataFlow(const Container<ValueType, Args...>& container) {
  return LazyPipeline<const ValueType, Container, Args...>(container);
}