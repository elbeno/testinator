#pragma once

#include "arbitrary.h"
#include <algorithm>
#include <string>

namespace testpp
{

  //------------------------------------------------------------------------------
  // specialization for string
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::basic_string<T>, false>
  {
    static const std::size_t N = 10;

    static std::basic_string<T> generate(std::size_t generation = 0)
    {
      if (generation < 3) generation = 3;
      std::basic_string<T> s;
      std::generate_n(std::back_inserter(s), N,
                      std::bind(Arbitrary<T>::generate, generation));
      return s;
    }

    static std::vector<std::basic_string<T>> shrink(const std::basic_string<T>& t)
    {
      std::vector<std::basic_string<T>> v(2);
      auto it = t.begin() + t.size()/2;
      copy(t.begin(), it, std::back_inserter(v[0]));
      copy(it, t.end(), std::back_inserter(v[1]));
      return v;
    }
  };

}
