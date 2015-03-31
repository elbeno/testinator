#pragma once

#include "arbitrary.h"

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
      return std::vector<std::pair<T1, T2>>{};
    }
  };

  //------------------------------------------------------------------------------
  // specialization for tuple
  //------------------------------------------------------------------------------
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

    static std::vector<std::tuple<Ts...>> shrink(const std::tuple<Ts...>&)
    {
      return std::vector<std::tuple<Ts...>>{};
    }
  };

}
