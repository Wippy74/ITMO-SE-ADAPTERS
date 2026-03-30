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

    struct SplitCache {
      GetterType getter;
      std::vector<ValueType> values;
      std::vector<ErrorType> errors;
      bool processed = false;
      
      explicit SplitCache(GetterType g) : getter(std::move(g)) {}
      
      void EnsureProcessed() {
        if (processed) {
          return;
        }
        processed = true;
        for (auto& result : getter()) {
          if (result.has_value()) {
            values.push_back(std::move(result.value()));
          } else {
            errors.push_back(std::move(result.error()));
          }
        }
      }
    };

    auto cache = std::make_shared<SplitCache>(data.State());

    auto error_stream = LazyPipeline<ErrorType, std::vector>(
      [cache](std::vector<ErrorType>& output) {
        cache->EnsureProcessed();
        output = cache->errors;
      }
    );
        
    auto value_stream = LazyPipeline<ValueType, std::vector>(
      [cache](std::vector<ValueType>& output) {
        cache->EnsureProcessed();
        output = cache->values;
      }
    );
    
    return std::pair{std::move(error_stream), std::move(value_stream)};
  }
};

template <template <typename, typename...> typename Container = SameCont, typename... Args>
auto SplitExpected() {
  return SplitExpectedAdapter<Container, Args...>();
}