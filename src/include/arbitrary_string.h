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
      std::basic_string<T> s;
      std::generate_n(std::back_inserter(s), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return s;
    }

    static std::vector<std::basic_string<T>> shrink(const std::basic_string<T>& t)
    {
      std::vector<std::basic_string<T>> v;
      if (t.size() < 2)
        return v;
      size_t l = t.size() / 2;
      v.push_back(t.substr(0, l));
      v.push_back(t.substr(l));
      return v;
    }
  };

}
