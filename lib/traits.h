#pragma once
#include <concepts>

template <typename T, typename ...Args>
struct SameCont {};

template<class Alloc>
concept IsAllocator = requires(Alloc alloc, std::size_t n) {
  { *alloc.allocate(n) } -> std::same_as<typename Alloc::value_type&>;
  { alloc.deallocate(alloc.allocate(n), n) };  
} && std::copy_constructible<Alloc> && std::equality_comparable<Alloc>;

template <typename T, template <typename, typename...> typename C, typename ...Args>
class LazyPipeline;

template <typename Alloc, template <typename, typename...> typename OtherCont, typename U>
struct ConvertParams {
  using Type = Alloc;
};

template <IsAllocator Alloc, template <typename, typename...> typename OtherCont, typename U>
struct ConvertParams<Alloc, OtherCont, U> {
  using Type = std::allocator_traits<Alloc>::template rebind_alloc<U>;
};

template <template<typename, typename...> typename OtherCont, typename U>
struct Traits {
  template <typename Head, typename ... Tail>
  struct Convert {
    using Type = Head;
  };

  template <typename T, template <typename, typename...> typename C, typename ...Head, typename Curr, typename ...Tail>
  requires ConvertParams<Curr, OtherCont, U>::Type
  struct Convert<LazyPipeline<T, C, Head...>, Curr, Tail...> {
    using ConvertedCurr = typename ConvertParams<Curr, OtherCont, U>::Type;
    using NewHead = LazyPipeline<T, C, Head..., ConvertedCurr>;
    using Type = typename Convert<NewHead, Tail...>::Type;
  };

  template <typename T, template <typename, typename...> typename C, typename ... Head, typename Curr, typename ...Tail>
  requires !(ConvertParams<Curr, OtherCont, U>::Type)
  struct Convert<LazyPipeline<T, C, Head..., >, Curr, Tail...> {
    using Type = typename Convert<LazyPipeline<T, C, Head...>, Tail...>::Type;
  };

  template <typename ...Args>
  using ToNewPipeline = typename Convert<LazyPipeline<U, OtherCont, Args...>::Type;
};