// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "arbitrary.h"

#include <algorithm>
#include <cstddef>
#include <string>

namespace testinator
{

  //------------------------------------------------------------------------------
  // specialization for string
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::basic_string<T>>
  {
    static const std::size_t N = 10;

    static std::basic_string<T> generate(
        std::size_t generation, unsigned long int randomSeed)
    {
      std::basic_string<T> s;
      if (generation == 0) return s;
      std::size_t n = N * ((generation / 100) + 1);
      s.reserve(n);
      std::generate_n(std::back_inserter(s), n,
                      [&] () { return Arbitrary<T>::generate(generation++, randomSeed++); });
      return s;
    }

    static std::basic_string<T> generate_n(std::size_t n, unsigned long int randomSeed)
    {
      std::basic_string<T> s;
      s.reserve(n);
      std::generate_n(std::back_inserter(s), n,
                      [&] () { return Arbitrary<T>::generate_n(n, randomSeed++); });
      return s;
    }

    static std::vector<std::basic_string<T>> shrink(const std::basic_string<T>& t)
    {
      std::vector<std::basic_string<T>> v;
      if (t.empty()) return v;
      auto l = t.size() / 2;
      v.push_back(t.substr(0, l));
      if (l > 0) v.push_back(t.substr(l));
      return v;
    }
  };

}
