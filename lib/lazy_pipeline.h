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

    std::function<void()> parent_invoke = [](){};

    Data(const Func& func) : cont_ptr_(new Container{}), func_ptr_(std::make_unique<Func>(func)), IsOwner_(true) {}
    Data(Container& c) : cont_ptr_(&c) {}
    ~Data() {
      if (IsOwner_) {
        delete cont_prt_;
      }
    }

    void Evaluate() {
      if (func_ptr_) {
        func_ptr_->operator()(*cont_ptr_);
        func_ptr_.reset();
      }
    }

    Container& Access(){
      Evaluate();
      return *cont_ptr;
    }
  private:
    Container* cont_ptr_;
    std::unique_ptr<Func> func_ptr_;
    bool IsOwner_;
  };

public:
  using Type = Data::Type;
  using value_type = Type;
  using Contrainer = Data::Container;

  LazyPipeline() : LazyPipeline(Contrainer{}) {}

  LazyPipeline(const Data::Func& func) : data_(std::make_shared<Data>(func)) {}

  template <typename Cont>
  requires std::same_as<const Cont, Contrainer>
  LazyPipeline(const Cont& c) : data_(std::make_shared<Data>(c)) {}

  const Contrainer& Access() const {
    data_->parent_invoke();
    return data_->Access();
  }

  std::function<void()> invoker() const {
    auto& data_ptr = data_;
    return [data_ptr]() {data_ptr->Access()};
  }

  auto begin() {
    return Access.begin();
  }
  
  auto end() {
    return Access.end();
  }

  auto begin() const {
    return Access.cbegin();
  }

  auto end() const {
    return Access.cend();
  }

  template <typename Adapt>
  auto operator|(Adapt adapter) {
    return adapter.Apply(*this);
  }

  std::function<Contrainer&()> State() const {
    auto& data_ptr = data_;
    auto& parent_inv = data_ptr->parent_invoke;
    return Container& [data_ptr, parent_inv]() {
      parent_inv();
      return data_ptr->Access();
    }
  }

private:
  std::shared_ptr<Data> data_;
};