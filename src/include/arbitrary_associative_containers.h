// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "arbitrary.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace testinator
{

  namespace
  {
    template <typename C>
    struct Arbitrary_Assoc
    {
      static const std::size_t N = 10;
      using V = typename C::value_type;

      static C generate(std::size_t generation, unsigned long int randomSeed)
      {
        C v;
        if (generation == 0) return v;
        std::size_t n = N * ((generation / 100) + 1);
        std::generate_n(
            std::inserter(v, v.begin()), n,
            [&] () { return Arbitrary<V>::generate(generation++, randomSeed++); });
        return v;
      }

      static C generate_n(std::size_t n, unsigned long int randomSeed)
      {
        C v;
        std::generate_n(
            std::inserter(v, v.begin()), n,
            [&] () { return Arbitrary<V>::generate_n(n, randomSeed++); });
        return v;
      }

      static std::vector<C> shrink(const C& c)
      {
        std::vector<C> v;
        if (c.empty()) return v;
        auto l = c.size() / 2;
        auto it = c.cbegin();
        v.push_back(C());
        for (decltype(l) count = 0; count < l; count++, it++)
          v[0].insert(*it);
        if (l > 0)
        {
          v.push_back(C());
          copy(it, c.cend(), std::inserter(v[1], v[1].begin()));
        }
        return v;
      }
    };
  }

  //------------------------------------------------------------------------------
  // specializations for set types
  //------------------------------------------------------------------------------
  template <typename T, typename Compare, typename Alloc>
  struct Arbitrary<std::set<T, Compare, Alloc>>
    : public Arbitrary_Assoc<std::set<T, Compare, Alloc>> {};

  template <typename T, typename Compare, typename Alloc>
  struct Arbitrary<std::multiset<T, Compare, Alloc>>
    : public Arbitrary_Assoc<std::multiset<T, Compare, Alloc>> {};

  template <typename T, typename Hash, typename KeyEq, typename Alloc>
  struct Arbitrary<std::unordered_set<T, Hash, KeyEq, Alloc>>
    : public Arbitrary_Assoc<std::unordered_set<T, Hash, KeyEq, Alloc>>
  {};

  //------------------------------------------------------------------------------
  // specializations for map types
  //------------------------------------------------------------------------------
  template <typename K, typename V, typename Compare, typename Alloc>
  struct Arbitrary<std::map<K, V, Compare, Alloc>>
    : public Arbitrary_Assoc<std::map<K, V, Compare, Alloc>> {};

  template <typename K, typename V, typename Compare, typename Alloc>
  struct Arbitrary<std::multimap<K, V, Compare, Alloc>>
    : public Arbitrary_Assoc<std::multimap<K, V, Compare, Alloc>>
  {};

  template <typename K, typename V, typename Hash, typename KeyEq, typename Alloc>
  struct Arbitrary<std::unordered_map<K, V, Hash, KeyEq, Alloc>>
    : public Arbitrary_Assoc<std::unordered_map<K, V, Hash, KeyEq, Alloc>>
  {};

}
