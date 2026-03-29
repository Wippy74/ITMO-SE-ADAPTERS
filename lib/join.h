#pragma once

#include "traits.h"
#include "processing.h"

#include <memory>
#include <optional>
#include <vector>

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;
  bool operator==(const KV& other) const = default;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;
  bool operator==(const JoinResult& other) const = default;
};


template <template <typename, typename...> typename Container, typename... Args>
struct TypeWrapper {};

template <typename RightFlow,
          typename GetRightKey,
          typename GetRightValue,
          typename GetLeftKey,
          typename GetLeftValue,
          template <typename, typename...> typename Container,
          typename... Args>
class JoinAdapter {
public:
  virtual ~JoinAdapter() = default;
  using RightKey = decltype(std::declval<GetRightKey>()(std::declval<typename RightFlow::Type>()));
  using RightValue = decltype(std::declval<GetRightValue>()(std::declval<typename RightFlow::Type>()));
private:
  struct RightData {
    RightFlow right_flow;
    GetRightKey get_right_key;
    GetRightValue get_right_value;
    std::function<typename RightFlow::Container&()> getter;
    RightData(const RightFlow& flow, const GetRightKey& get_key, const GetRightValue& get_value)
      : right_flow(flow),
      get_right_key(get_key),
      get_right_value(get_value) {}

    void Init() {
      getter = right_flow.State();
    }
  };

  struct LeftFuncs {
    GetLeftKey get_left_key;
    GetLeftValue get_left_value;
    LeftFuncs(const GetLeftKey& get_key, const GetLeftValue& get_value) : get_left_key(get_key), get_left_value(get_value) {}
  };
  std::shared_ptr<RightData> right_data_ptr_;
  std::shared_ptr<LeftFuncs> left_funcs_ptr_;
public:
    template <typename LeftFlow>
    using LeftKey = decltype(left_funcs_ptr_->get_left_key(std::declval<typename LeftFlow::Type>()));

    template <typename LeftFlow>
    using LeftValue = decltype(left_funcs_ptr_->get_left_value(std::declval<typename LeftFlow::Type>()));

    template <typename LeftFlow>
    using Result = JoinResult<LeftValue<LeftFlow>, RightValue>;

    JoinAdapter(TypeWrapper<Container, Args...>,
                const RightFlow& right_flow,
                GetRightKey get_right_key,
                GetLeftKey get_left_key,
                GetRightValue get_right_value,
                GetLeftValue get_left_value)
      : right_data_ptr_(std::make_shared<RightData>(right_flow, get_right_key, get_right_value)),
      left_funcs_ptr_(std::make_shared<LeftFuncs>(get_left_key, get_left_value)) {}

    template <typename LeftFlow>
    auto Apply(LeftFlow& left_flow) {
      using ThisResult = Result<LeftFlow>;

      auto right_data_ptr = right_data_ptr_;
      auto left_funcs_ptr = left_funcs_ptr_;
      right_data_ptr->Init();

      auto next_flow = left_flow.template Make<ThisResult, std::vector>(
        [right_data_ptr, left_funcs_ptr](auto& left_container, auto& new_container) {
            for (auto& left_item : left_container) {
              const auto& left_key = left_funcs_ptr->get_left_key(left_item);
              const auto& left_value = left_funcs_ptr->get_left_value(left_item);
              bool found = false;
              for (auto& right_item : right_data_ptr->getter()) {
                const auto& right_key = right_data_ptr->get_right_key(right_item);
                if (left_key == right_key) {
                  const auto& right_value = right_data_ptr->get_right_value(right_item);
                  new_container.push_back(ThisResult{left_value, right_value});
                  found = true;
                }
              }
              if (!found) {
                new_container.push_back(ThisResult{left_value, std::nullopt});
              }
            }
        });
      right_data_ptr->right_flow.ParentDerived(next_flow);
      return next_flow;
    }
};

template <template <typename, typename...> typename Container = SameCont, typename... Args, typename RightFlow>
auto Join(RightFlow& right_flow) {
  return JoinAdapter(TypeWrapper<Container, Args...>(), right_flow,
    [](const auto& kv) { return kv.key; },
    [](const auto& kv) { return kv.key; },
    [](const auto& kv) { return kv.value; },
    [](const auto& kv) { return kv.value; }
  );
}

template <template <typename, typename...> typename Container = SameCont, typename... Args, typename RightFlow,
          typename GetLeftKey,
          typename GetRightKey>
auto Join(RightFlow& right_flow, const GetLeftKey& get_left_key, const GetRightKey& get_right_key) {
  return JoinAdapter(TypeWrapper<Container, Args...>(), right_flow, get_right_key, get_left_key,
    [](const auto& v) { return v; },
    [](const auto& v) { return v; }
  );
}