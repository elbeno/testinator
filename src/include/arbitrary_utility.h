#pragma once

#include "arbitrary.h"

#include "prettyprint.h"
#include <tuple>
#include <utility>

namespace testinator
{

  //------------------------------------------------------------------------------
  // specialization for pair
  //------------------------------------------------------------------------------
  template <typename T1, typename T2>
  struct Arbitrary<std::pair<T1, T2>>
  {
    static std::pair<T1, T2> generate(std::size_t generation, unsigned long int randomSeed)
    {
      return std::pair<T1, T2>(
          Arbitrary<T1>::generate(generation, randomSeed),
          Arbitrary<T2>::generate(generation, randomSeed));
    }

    static std::pair<T1, T2> generate_n(std::size_t n, unsigned long int randomSeed)
    {
      return std::pair<T1, T2>(
          Arbitrary<T1>::generate_n(n, randomSeed),
          Arbitrary<T2>::generate_n(n, randomSeed));
    }

    static std::vector<std::pair<T1, T2>> shrink(const std::pair<T1, T2>&)
    {
      // TODO: cartesian product?
      return std::vector<std::pair<T1, T2>>{};
    }
  };

  //------------------------------------------------------------------------------
  // specialization for tuple
  //------------------------------------------------------------------------------
  template <typename ...Ts, std::size_t ...Is>
  auto tuple_tail(const std::tuple<Ts...>& t, std::index_sequence<Is...>)
  {
    return std::make_tuple(std::get<Is + 1>(t)...);
  }

  template <typename ...Ts>
  auto tuple_tail(const std::tuple<Ts...>& t)
  {
    return tuple_tail(t, std::make_index_sequence<sizeof...(Ts) - 1>());
  }

  template <typename ...Ts>
  struct Arbitrary<std::tuple<Ts...>>
  {
    static std::tuple<Ts...> generate(std::size_t generation, unsigned long int randomSeed)
    {
      return std::make_tuple(Arbitrary<Ts>::generate(generation, randomSeed)...);
    }

    static std::tuple<Ts...> generate_n(std::size_t n, unsigned long int randomSeed)
    {
      return std::make_tuple(Arbitrary<Ts>::generate_n(n, randomSeed)...);
    }

    static std::vector<std::tuple<Ts...>> shrink(const std::tuple<Ts...>& t)
    {
      std::vector<std::tuple<Ts...>> ret{};
      // TODO: cartesian product?
      using T = std::tuple_element_t<0, std::tuple<Ts...>>;
      std::vector<T> v = Arbitrary<T>::shrink(std::get<0>(t));
      for (auto&& e : v)
      {
        ret.push_back(std::tuple_cat(std::tuple<T>{e}, tuple_tail(t)));
      }
      return ret;
    }
  };

}
