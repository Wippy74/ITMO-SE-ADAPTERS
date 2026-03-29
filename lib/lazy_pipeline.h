#pragma once

#include <memory>
#include <functional>
#include <string>

#include "traits.h"

template <typename T, template <typename, typename...> typename C, typename ...Args>
class LazyPipeline {
private:
  class Data {
  private:
    template <typename U>
		struct ContainerType{
			using Type = C<T, Args...>;
		};

		template <typename U> 
		requires std::same_as<U, const std::remove_const_t<U>>
		struct ContainerType<U>{
			using Type = const C<std::remove_const_t<U>, Args...>;
		};

  public:
    using Type = T;
    using Container = ContainerType<T>::Type;
    using Func = std::function<void(Container&)>;

    std::function<void()> parent_invoke = [](){};

    Data(const Func& func) : cont_ptr_(new Container{}), func_ptr_(std::make_unique<Func>(func)), IsOwner_(true) {}

    Data(Container& c) : cont_ptr_(&c) {}

    Data(Container&& c): cont_ptr_(new Container(std::move(c))) , IsOwner_(true) {}

    Data(Data&) = delete;

    ~Data() {
      if (IsOwner_) {
        delete cont_ptr_;
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
      return *cont_ptr_;
    }
  private:
    Container* cont_ptr_;
    std::unique_ptr<Func> func_ptr_;
    bool IsOwner_;
  };

  template <typename U, template <typename, typename...> typename C2, typename ...Args2>
  struct RebindingStruct {
    using Type = Traits<C2, U>::template ToNewPipeline<Args2...>;
  };

  template <typename U>
  struct RebindingStruct<U, SameCont> {
    using Type = Traits<C, U>::template ToNewPipeline<Args...>;
  };

public:
  using Type = Data::Type;
  using value_type = Type;
  using Container = Data::Container;

  LazyPipeline() : LazyPipeline(Container{}) {}

  LazyPipeline(const typename Data::Func& func) : data_(std::make_shared<Data>(func)) {}

  template <typename Cont>
  requires std::same_as<const Cont, Container>
  LazyPipeline(const Cont& c) : data_(std::make_shared<Data>(c)) {}

  template <typename Cont>
	requires std::same_as<std::remove_reference_t<Cont>, Container>
	LazyPipeline(Cont&& c) : data_(std::make_shared<Data>(std::forward<Cont>(c))) {}

  template <typename U, template <typename, typename ...> typename C2, typename ...Args2>
  using Rebind = RebindingStruct<U, C2, Args2...>::Type;

  template <typename NextFlow>
  using NextFunc = std::function<void(Container&, typename NextFlow::Container&)>;

  const Container& Access() const {
    data_->parent_invoke();
    return data_->Access();
  }

  std::function<void()> invoker() const {
    auto& data_ptr = data_;
    return [data_ptr]() {data_ptr->Access();};
  }

  auto begin() {
    return Access().begin();
  }
  
  auto end() {
    return Access().end();
  }

  auto begin() const {
    return Access().cbegin();
  }

  auto end() const {
    return Access().cend();
  }

  template <typename Adapt>
  auto operator|(Adapt adapter) {
    return adapter.Apply(*this);
  }

  std::function<Container&()> State() const {
    auto& data_ptr = data_;
    auto& parent_inv = data_ptr->parent_invoke;
    return [data_ptr, parent_inv]() -> Container&{
      parent_inv();
      return data_ptr->Access();
    };
  }

  template <typename NextFlow>
  void ParentDerived(NextFlow f) {
    data_->parent_invoke = f.invoker();
  }

  void ParentDerived(std::function<void()> inv) {
    data_->parent_invoke = inv;
  }

  template <typename U, template <typename, typename ...> typename C2, typename ...Args2>
  auto Make(const NextFunc<Rebind<U, C2, Args2...>>& convert) {
    auto getter = State();
    auto next_flow = Rebind<U, C2, Args2...>([getter, convert](auto& next_cont) {convert(getter(), next_cont);});
    ParentDerived(next_flow);
    return next_flow;
  }
private:
  std::shared_ptr<Data> data_;
};