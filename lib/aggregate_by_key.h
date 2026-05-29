#pragma once

#include "lazy_pipeline.h"
#include "traits.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <utility>

template <typename AggregateType, typename AggregateFunc, typename KeyExtractor, template <typename, typename...> typename Container, typename... Args>
class AggregateByKeyAdapter {
private:
  template <typename Key, typename Value>
  class KeyIndexMap {
  private:
    std::unordered_map<Key, size_t> key_idx;
    AggregateType init_val_;
    AggregateFunc aggregate_func_;
    KeyExtractor extract_key_;
  public:
    KeyIndexMap(AggregateByKeyAdapter& parent) : init_val_(parent.init_val_), aggregate_func_(parent.aggregate_func_), 
                                                 extract_key_(parent.extract_key_) {}
    template <typename TargetContainer>
    void Aggregate(TargetContainer& container, Value& value) {
      Key key = extract_key_(value);
      auto it = key_idx.find(key);
      if (it != key_idx.end()) {
        aggregate_func_(value, container[it->second].second);
      } else {
        size_t index = container.size();
        key_idx.emplace(key, index);
        container.push_back(std::pair{key, init_val_});
        aggregate_func_(value, container[index].second);
      }
    }
  };
  AggregateType init_val_;
  AggregateFunc aggregate_func_;
  KeyExtractor extract_key_;
public:
  AggregateByKeyAdapter(const AggregateType& initial_value,
                        const AggregateFunc& accumulate,
                        const KeyExtractor& extract_key) : init_val_(initial_value), aggregate_func_(accumulate), extract_key_(extract_key) {}
  template <typename Data>
  using KeyType = decltype(std::declval<KeyExtractor>()(std::declval<typename Data::Type&>()));

  template <typename Data>
  using ResultType = std::pair<KeyType<Data>, AggregateType>;

  template <typename Data>
  auto Apply(Data& data) {
    auto key_index_map = std::make_shared<KeyIndexMap<KeyType<Data>, typename Data::Type>>(*this);
    return data.template Make<ResultType<Data>, Container, Args...>([key_index_map](auto& input, auto& output) {
                                                                      for (auto& value : input) {
                                                                        key_index_map->Aggregate(output, value);
                                                                      }
                                                                    });
  }
};

template <
  template <typename, typename...> typename Container = SameCont,
  typename... Args, typename AggregateType, typename AggregateFunc, typename KeyExtractor>
auto AggregateByKey(AggregateType initial_value, AggregateFunc accumulate, KeyExtractor extract_key) {
  return AggregateByKeyAdapter<AggregateType, AggregateFunc, KeyExtractor, Container, Args...>(initial_value, accumulate, extract_key);
}