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
  using type = Alloc;
};

template <IsAllocator Alloc, template <typename, typename...> typename OtherCont, typename U>
struct ConvertParams<Alloc, OtherCont, U> {
  using type = std::allocator_traits<Alloc>::template rebind_alloc<U>;
};
