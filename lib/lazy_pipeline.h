#pragma once

#include <memory>
#include <functional>
#include <string>

template <typename T, template <typename, typename...> typename C, typename ...Args>
class LazyPipeline {
private:
  class Data {
  public:
    using Type = T;
    if constexpr (std::is_const<T>) {
      using Container = const C<std::remove_const_t<T>, Args...>;
    } else {
      using Container = C<T, Args...>;
    }
    using Func = std::function<void>(C&);
    Data(const Func& func) : cont_ptr_(new Container{}), func_ptr_(std::make_unique<Func>(func)), IsOwner_(true) {}
    Data(Container& c) : cont_ptr_(&c) {}
    ~Data() {
      if (IsOwner_) {
        delete cont_prt_;
      }
    }
  private:
    Container* cont_ptr_;
    std::unique_ptr<Func> func_ptr_;
    bool IsOwner_;
  }
private:
  std::shared_ptr<Data> data_;
};