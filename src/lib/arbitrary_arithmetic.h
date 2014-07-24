#pragma once

#include "arbitrary.h"
#include <limits>
#include <random>

namespace testpp
{

  //------------------------------------------------------------------------------
  // specialization for arithmetic types
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<T, true>
  {
    static T generate(std::size_t generation = 0)
    {
      switch (generation)
      {
        case 0:
          return 0;
        case 1:
          return std::numeric_limits<T>::min();
        case 2:
          return std::numeric_limits<T>::max();

          // when generating containers, their contents are always on generation
          // 3 at least, so that arithmetic types are randomly generated inside
        default:
        {
          std::random_device rd;
          std::mt19937 gen(rd());
          std::uniform_int_distribution<T> dis(
              std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
          return dis(gen);
        }
      }
    }

    static std::vector<T> shrink(const T&)
    {
      return std::vector<T>();
    }
  };

}
