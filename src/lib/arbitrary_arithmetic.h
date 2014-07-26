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

  //------------------------------------------------------------------------------
  // specialization for bool
  //------------------------------------------------------------------------------
  template <>
  struct Arbitrary<bool, true>
  {
    static bool generate(std::size_t generation = 0)
    {
      return (generation & 1) == 0;
    }

    static std::vector<bool> shrink(const bool&)
    {
      return std::vector<bool>();
    }
  };

  //------------------------------------------------------------------------------
  // specialization for char
  //------------------------------------------------------------------------------
  template <>
  struct Arbitrary<char, true>
  {
    static char generate(std::size_t generation = 0)
    {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<char> dis(32, 126);
      return dis(gen);
    }

    static std::vector<char> shrink(const char&)
    {
      return std::vector<char>();
    }
  };

}
