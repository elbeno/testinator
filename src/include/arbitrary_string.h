// Copyright (c) 2014-2016 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "arbitrary.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>

namespace testinator
{

  //------------------------------------------------------------------------------
  // specialization for string
  //------------------------------------------------------------------------------
  template <typename T, typename CharTraits, typename Alloc>
  struct Arbitrary<std::basic_string<T, CharTraits, Alloc>>
  {
    using output_type = std::basic_string<T, CharTraits, Alloc>;
    
    static const std::size_t N = 5;

    static output_type generate(
        std::size_t generation, unsigned long int randomSeed)
    {
      output_type s;
      if (generation == 0) return s;
      std::size_t n = N * ((generation / 100) + 1);
      s.reserve(n);
      std::generate_n(std::back_inserter(s), n,
                      [&] () { return Arbitrary<T>::generate(generation++, randomSeed++); });
      return s;
    }

    static output_type generate_n(std::size_t n, unsigned long int randomSeed)
    {
      output_type s;
      s.reserve(n);
      std::generate_n(std::back_inserter(s), n,
                      [&] () { return Arbitrary<T>::generate_n(n, randomSeed++); });
      return s;
    }

    static std::vector<output_type> shrink(const output_type& t)
    {
      std::vector<output_type> v;
      if (t.empty()) return v;
      auto l = t.size() / 2;
      v.push_back(t.substr(0, l));
      if (l > 0) v.push_back(t.substr(l));
      return v;
    }
  };

}
