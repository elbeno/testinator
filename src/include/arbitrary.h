#pragma once

#include <cstddef>
#include <vector>

namespace testpp
{

  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary
  {
    // Generates an instance of a T. The generation parameter in some way
    // controls the "complexity" of T, if applicable; for instance, a higher
    // generation implies a larger container in some way.
    static T generate(std::size_t /*generation*/, unsigned long int /*randomSeed*/)
    { return T(); }

    // Generates an instance of a T of EXACT complexity n for use in complexity
    // properties. For instance, a string of length n. This is used mostly for
    // container types.
    static T generate_n(std::size_t /*n*/, unsigned long int /*randomSeed*/)
    { return T(); }

    // Given a T, tries to shrink it, generating a vector of Ts derived from it.
    // Higher precedence items (more aggressively shrunk) should be near the
    // front of the vector.
    static std::vector<T> shrink(const T& /*t*/)
    { return std::vector<T>(); }
  };

  template <typename T>
  struct Arbitrary<const T> : public Arbitrary<T> {};

}

#include "arbitrary_arithmetic.h"
#include "arbitrary_associative_containers.h"
#include "arbitrary_sequence_containers.h"
#include "arbitrary_string.h"
