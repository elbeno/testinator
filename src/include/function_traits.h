#pragma once

#include <chrono>
#include <tuple>
#include <utility>

namespace testinator
{

  //------------------------------------------------------------------------------
  template <typename T>
  struct function_traits
    : public function_traits<decltype(&T::operator())>
  {};

  template <typename R, typename... A>
  struct function_traits<R(A...)>
  {
    using argTuple = std::tuple<std::decay_t<A>...>;

    template <typename F>
    static R apply(const F& f, const argTuple& t)
    {
      return unpackApply(f, t, std::index_sequence_for<A...>());
    }

    template <typename F, std::size_t... Is>
    static R unpackApply(const F& f, const argTuple& t, std::index_sequence<Is...>)
    {
      return f(std::get<Is>(t)...);
    }

    template <typename F>
    static auto apply_timed(std::size_t num, const F& f, const argTuple& t)
    {
      return unpackApply_timed(num, f, t, std::index_sequence_for<A...>());
    }

    template <typename F, std::size_t... Is>
    static auto unpackApply_timed(std::size_t num, const F& f,
                                  const argTuple& t, std::index_sequence<Is...>)
    {
      return apply_timed_(num, f, std::get<Is>(t)...);
    }

    template <typename F, typename...Ts>
    static auto apply_timed_(std::size_t num, F&& f, Ts&&... ts)
    {
      auto t1 = std::chrono::high_resolution_clock::now();
      for (std::size_t i = 0; i < num; ++i)
      {
        std::forward<F>(f)(std::forward<Ts>(ts)...);
      }
      auto t2 = std::chrono::high_resolution_clock::now();
      return t2 - t1;
    }
  };

  template <typename C, typename R, typename... A>
  struct function_traits<R(C::*)(A...)>
    : public function_traits<R(A...)>
  {};

  template <typename C, typename R, typename... A>
  struct function_traits<R(C::*)(A...) const>
    : public function_traits<R(A...)>
  {};
}
