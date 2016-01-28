// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "arbitrary.h"
#include "test.h"

#include <cstddef>
#include <limits>
#include <random>

namespace testinator
{

  namespace detail
  {
    template <typename T>
    struct Arbitrary_Arithmetic
    {
      static T generate(std::size_t generation, unsigned long int randomSeed)
      {
        switch (generation)
        {
          case 0:
            return 0;
          case 1:
            return std::numeric_limits<T>::min();
          case 2:
            return std::numeric_limits<T>::max();

          default:
          {
            std::mt19937& gen = testinator::GetTestRegistry().RNG();
            gen.seed(randomSeed);
            std::uniform_int_distribution<T> dis(
                std::numeric_limits<T>::min() + 1, std::numeric_limits<T>::max() - 1);
            return dis(gen);
          }
        }
      }

      static T generate_n(std::size_t n, unsigned long int randomSeed)
      {
        return generate(n, randomSeed);
      }

      static std::vector<T> shrink(const T&)
      {
        return std::vector<T>();
      }
    };

    template <typename T>
    struct Arbitrary_Arithmetic_IntCast
    {
      static T generate(std::size_t generation, unsigned long int randomSeed)
      {
        switch (generation)
        {
          case 0:
            return 0;
          case 1:
            return std::numeric_limits<T>::min();
          case 2:
            return std::numeric_limits<T>::max();

          default:
          {
            std::mt19937& gen = testinator::GetTestRegistry().RNG();
            gen.seed(randomSeed);
            std::uniform_int_distribution<int> dis(
                std::numeric_limits<T>::min() + 1, std::numeric_limits<T>::max() - 1);
            return static_cast<T>(dis(gen));
          }
        }
      }

      static T generate_n(std::size_t n, unsigned long int randomSeed)
      {
        return generate(n, randomSeed);
      }

      static std::vector<T> shrink(const T&)
      {
        return std::vector<T>();
      }
    };

    template <typename T>
    struct Arbitrary_Arithmetic_Real
    {
      static T generate(std::size_t generation, unsigned long int randomSeed)
      {
        switch (generation)
        {
          case 0:
            return 0;
          case 1:
            return std::numeric_limits<T>::min();
          case 2:
            return std::numeric_limits<T>::max();
          case 3:
            return std::numeric_limits<T>::lowest();

          default:
          {
            std::mt19937& gen = testinator::GetTestRegistry().RNG();
            gen.seed(randomSeed);
            std::uniform_real_distribution<T> dis(
                std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
            return dis(gen);
          }
        }
      }

      static T generate_n(std::size_t n, unsigned long int randomSeed)
      {
        return generate(n, randomSeed);
      }

      static std::vector<T> shrink(const T&)
      {
        return std::vector<T>();
      }
    };

  }

  //------------------------------------------------------------------------------
  // specializations for arithmetic types
  //------------------------------------------------------------------------------
  template <>
  struct Arbitrary<signed char> : public detail::Arbitrary_Arithmetic_IntCast<signed char> {};

  template <>
  struct Arbitrary<unsigned char> : public detail::Arbitrary_Arithmetic_IntCast<unsigned char> {};

  template <>
  struct Arbitrary<wchar_t> : public detail::Arbitrary_Arithmetic_IntCast<wchar_t> {};

// MSVC does not yet fully support char16_t or char32_t.
#ifndef _MSC_VER
  template <>
  struct Arbitrary<char16_t> : public detail::Arbitrary_Arithmetic<char16_t> {};

  template <>
  struct Arbitrary<char32_t> : public detail::Arbitrary_Arithmetic<char32_t> {};
#endif

  template <>
  struct Arbitrary<short> : public detail::Arbitrary_Arithmetic<short> {};

  template <>
  struct Arbitrary<unsigned short> : public detail::Arbitrary_Arithmetic<unsigned short> {};

  template <>
  struct Arbitrary<int> : public detail::Arbitrary_Arithmetic<int> {};

  template <>
  struct Arbitrary<unsigned int> : public detail::Arbitrary_Arithmetic<unsigned int> {};

  template <>
  struct Arbitrary<long> : public detail::Arbitrary_Arithmetic<long> {};

  template <>
  struct Arbitrary<unsigned long> : public detail::Arbitrary_Arithmetic<unsigned long> {};

  template <>
  struct Arbitrary<long long> : public detail::Arbitrary_Arithmetic<long long> {};

  template <>
  struct Arbitrary<unsigned long long> : public detail::Arbitrary_Arithmetic<unsigned long long> {};

  template <>
  struct Arbitrary<float> : public detail::Arbitrary_Arithmetic_Real<float> {};

  template <>
  struct Arbitrary<double> : public detail::Arbitrary_Arithmetic_Real<double> {};

  template <>
  struct Arbitrary<long double> : public detail::Arbitrary_Arithmetic_Real<long double> {};

  //------------------------------------------------------------------------------
  // specialization for bool
  //------------------------------------------------------------------------------
  template <>
  struct Arbitrary<bool>
  {
    static bool generate(std::size_t generation, unsigned long int)
    {
      return (generation & 1) == 0;
    }

    static bool generate_n(std::size_t n, unsigned long int randomSeed)
    {
      return generate(n, randomSeed);
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
  struct Arbitrary<char>
  {
    static char generate(std::size_t, unsigned long int randomSeed)
    {
      std::mt19937& gen = testinator::GetTestRegistry().RNG();
      gen.seed(randomSeed);
      std::uniform_int_distribution<int> dis(32, 126);
      return static_cast<char>(dis(gen));
    }

    static char generate_n(std::size_t n, unsigned long int randomSeed)
    {
      return generate(n, randomSeed);
    }

    static std::vector<char> shrink(const char&)
    {
      return std::vector<char>();
    }
  };

}
