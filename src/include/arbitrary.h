#pragma once

#include <vector>

namespace testpp
{

  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary
  {
    // Generates an instance of a T. The generation parameter in some way
    // controls the "complexity" of T, if applicable, for instance, a higher
    // generation implies a larger container in some way.
    static T generate(std::size_t generation) { return T(); }

    // Given a T, tries to shrink it, generating a vector of Ts derived from it.
    // Higher precedence items (more aggressively shrunk) should be near the
    // front of the vector.
    static std::vector<T> shrink(const T& t) { return std::vector<T>(); }
  };

  template <typename T>
  struct Arbitrary<const T> : public Arbitrary<T> {};

}

#include "arbitrary_arithmetic.h"
#include "arbitrary_associative_containers.h"
#include "arbitrary_sequence_containers.h"
#include "arbitrary_string.h"
