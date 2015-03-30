#pragma once

#include "arbitrary.h"

#include <utility>

namespace testpp
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

    static std::vector<std::pair<T1, T2>> shrink(const std::pair<T1, T2>& t)
    {
      std::vector<T1> v1 = Arbitrary<T1>::shrink(t.first);
      std::vector<T2> v2 = Arbitrary<T2>::shrink(t.second);
      std::vector<std::pair<T1, T2>> v;
      auto it1 = v1.cbegin();
      auto it2 = v2.cbegin();
      for (; it1 != v1.cend() && it2 != v2.cend(); ++it1, ++it2)
        v.push_back(std::pair<T1, T2>(*it1, *it2));
      return v;
    }
  };

}
