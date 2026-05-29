#pragma once

#include "lazy_pipeline.h"
#include "traits.h"

#include <utility>

template <typename T, template <typename, typename...> typename Container, typename... Args>
auto AsDataFlow(Container<T, Args...>& container) {
  return LazyPipeline<T, Container, Args...>(container);
}

template <typename T, template <typename, typename...> typename Container, typename... Args>
auto AsDataFlow(const Container<T, Args...>& container) {
  Container<T, Args...> copy(container);
  return LazyPipeline<T, Container, Args...>(std::move(copy));
}