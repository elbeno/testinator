#pragma once

#include <type_traits>
#include <vector>

namespace testpp
{

  //------------------------------------------------------------------------------
  template <typename T, bool = std::is_arithmetic<T>::value>
  struct Arbitrary
  {
    // Generates an instance of a T. The generation parameter in some way
    // controls the "complexity" of T, if applicable, for instance, a higher
    // generation implies a larger container in some way.
    static T generate(std::size_t generation);

    // Given a T, tries to shrink it, generating a vector of Ts derived from it.
    // Higher precedence items (more aggressively shrunk) should be near the
    // front of the vector.
    static std::vector<T> shrink(const T& t) { return std::vector<T>(); }
  };

  template <typename T>
  struct Arbitrary<const T, false> : public Arbitrary<T, false> {};
  template <typename T>
  struct Arbitrary<const T, true> : public Arbitrary<T, true> {};

}

#include "arbitrary_arithmetic.h"
#include "arbitrary_containers.h"
#include "arbitrary_string.h"
