#pragma once

#include "arbitrary.h"
#include <limits>
#include <random>

namespace testpp
{

  namespace
  {
    template <typename T>
    struct Arbitrary_Arithmetic
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

      static T generate_n(std::size_t n)
      {
        return generate(n);
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
  struct Arbitrary<signed char> : public Arbitrary_Arithmetic<signed char> {};

  template <>
  struct Arbitrary<unsigned char> : public Arbitrary_Arithmetic<unsigned char> {};

  template <>
  struct Arbitrary<wchar_t> : public Arbitrary_Arithmetic<wchar_t> {};

  template <>
  struct Arbitrary<char16_t> : public Arbitrary_Arithmetic<char16_t> {};

  template <>
  struct Arbitrary<char32_t> : public Arbitrary_Arithmetic<char32_t> {};

  template <>
  struct Arbitrary<short> : public Arbitrary_Arithmetic<short> {};

  template <>
  struct Arbitrary<unsigned short> : public Arbitrary_Arithmetic<unsigned short> {};

  template <>
  struct Arbitrary<int> : public Arbitrary_Arithmetic<int> {};

  template <>
  struct Arbitrary<unsigned int> : public Arbitrary_Arithmetic<unsigned int> {};

  template <>
  struct Arbitrary<long> : public Arbitrary_Arithmetic<long> {};

  template <>
  struct Arbitrary<unsigned long> : public Arbitrary_Arithmetic<unsigned long> {};

  template <>
  struct Arbitrary<long long> : public Arbitrary_Arithmetic<long long> {};

  template <>
  struct Arbitrary<unsigned long long> : public Arbitrary_Arithmetic<unsigned long long> {};

  template <>
  struct Arbitrary<float> : public Arbitrary_Arithmetic<float> {};

  template <>
  struct Arbitrary<double> : public Arbitrary_Arithmetic<double> {};

  template <>
  struct Arbitrary<long double> : public Arbitrary_Arithmetic<long double> {};

  //------------------------------------------------------------------------------
  // specialization for bool
  //------------------------------------------------------------------------------
  template <>
  struct Arbitrary<bool>
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
  struct Arbitrary<char>
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
