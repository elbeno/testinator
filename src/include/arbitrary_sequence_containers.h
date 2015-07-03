// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "arbitrary.h"

#include <algorithm>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

namespace testinator
{

  namespace
  {
    template <typename C>
    struct Arbitrary_RandomSequence
    {
      static const std::size_t N = 10;
      typedef typename C::value_type V;

      static C generate(std::size_t generation, unsigned long int randomSeed)
      {
        C v;
        if (generation == 0) return v;
        std::size_t n = N * ((generation / 100) + 1);
        std::generate_n(
            std::back_inserter(v), n,
            [&] () { return Arbitrary<V>::generate(generation++, randomSeed++); });
        return v;
      }

      static C generate_n(std::size_t n, unsigned long int randomSeed)
      {
        C v;
        std::generate_n(
            std::back_inserter(v), n,
            [&] () { return Arbitrary<V>::generate_n(n, randomSeed++); });
        return v;
      }

      static std::vector<C> shrink(const C& c)
      {
        std::vector<C> v;
        if (c.empty()) return v;
        auto l = c.size() / 2;
        auto it = c.cbegin();
        std::advance(it, static_cast<typename C::difference_type>(l));
        v.push_back(C{c.cbegin(), it});
        if (l > 0) v.push_back(C{it, c.cend()});
        return v;
      }
    };
  }

  //------------------------------------------------------------------------------
  // specializations for sequence containers
  //------------------------------------------------------------------------------
  template <typename T, typename Alloc>
  struct Arbitrary<std::vector<T, Alloc>> : public Arbitrary_RandomSequence<std::vector<T, Alloc>> {};

  template <typename T, typename Alloc>
  struct Arbitrary<std::deque<T, Alloc>> : public Arbitrary_RandomSequence<std::deque<T, Alloc>> {};

  //------------------------------------------------------------------------------
  // specialization for list
  //------------------------------------------------------------------------------
  template <typename T, typename Alloc>
  struct Arbitrary<std::list<T, Alloc>>
  {
    typedef std::list<T, Alloc> output_type;
    
    static const std::size_t N = 10;

    static output_type generate(std::size_t generation, unsigned long int randomSeed)
    {
      output_type v;
      if (generation == 0) return v;
      std::size_t n = N * ((generation / 100) + 1);
      std::generate_n(std::back_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate(generation++, randomSeed++); });
      return v;
    }

    static output_type generate_n(std::size_t n, unsigned long int randomSeed)
    {
      output_type v;
      std::generate_n(std::back_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate_n(n, randomSeed++); });
      return v;
    }

    static std::vector<output_type> shrink(const output_type& t)
    {
      std::vector<output_type> v;
      if (t.empty()) return v;
      auto l = t.size() / 2;
      auto it = t.cbegin();
      v.push_back(output_type());
      for (decltype(l) count = 0; count < l; count++, it++)
        v[0].push_back(*it);
      if (l > 0)
      {
        v.push_back(output_type());
        copy(it, t.cend(), std::back_inserter(v[1]));
      }
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for forward_list
  //------------------------------------------------------------------------------
  template <typename T, typename Alloc>
  struct Arbitrary<std::forward_list<T, Alloc>>
  {
    typedef std::forward_list<T, Alloc> output_type;
    
    static const std::size_t N = 10;

    static output_type generate(std::size_t generation, unsigned long int randomSeed)
    {
      output_type v;
      if (generation == 0) return v;
      std::size_t n = N * ((generation / 100) + 1);
      std::generate_n(std::front_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate(generation++, randomSeed++); });
      return v;
    }

    static output_type generate_n(std::size_t n, unsigned long int randomSeed)
    {
      output_type v;
      std::generate_n(std::front_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate_n(n, randomSeed++); });
      return v;
    }

    static std::vector<output_type> shrink(const output_type& t)
    {
      std::vector<output_type> v;
      if (t.empty()) return v;
      auto l = std::distance(t.cbegin(), t.cend()) / 2;
      auto it = t.cbegin();
      v.push_back(output_type());
      for (auto count = 0; count < l; count++, it++)
        v[0].push_front(*it);
      if (l > 0)
      {
        v.push_back(output_type());
        copy(it, t.cend(), std::front_inserter(v[1]));
      }
      return v;
    }
  };

}
