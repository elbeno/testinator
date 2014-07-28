#pragma once

#include "arbitrary.h"
#include <algorithm>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace testpp
{

  namespace
  {
    template <typename C>
    struct Arbitrary_Assoc
    {
      static const std::size_t N = 10;
      typedef typename C::value_type V;

      static C generate(std::size_t generation = 0)
      {
        C v;
        std::size_t n = N * ((generation / 100) + 1);
        std::generate_n(
            std::inserter(v, v.begin()), n,
            [&] () { return Arbitrary<V>::generate(generation++); });
        return v;
      }

      static C generate_n(std::size_t n)
      {
        C v;
        std::generate_n(
            std::inserter(v, v.begin()), n,
            [&] () { return Arbitrary<V>::generate(n); });
        return v;
      }

      static std::vector<C> shrink(const C& c)
      {
        std::vector<C> v;
        if (c.size() < 2)
          return v;
        auto it = c.begin();
        v.push_back(C());
        for (std::size_t count = 0; count < c.size()/2; count++, it++)
          v[0].insert(*it);
        v.push_back(C());
        copy(it, c.end(), std::inserter(v[1], v[1].begin()));
        return v;
      }
    };
  }

  //------------------------------------------------------------------------------
  // specializations for set types
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::set<T>> : public Arbitrary_Assoc<std::set<T>> {};

  template <typename T>
  struct Arbitrary<std::multiset<T>> : public Arbitrary_Assoc<std::multiset<T>> {};

  template <typename T>
  struct Arbitrary<std::unordered_set<T>> : public Arbitrary_Assoc<std::unordered_set<T>> {};

  //------------------------------------------------------------------------------
  // specialization for pair
  //------------------------------------------------------------------------------
  template <typename T1, typename T2>
  struct Arbitrary<std::pair<T1, T2>>
  {
    static std::pair<T1, T2> generate(std::size_t generation = 0)
    {
      return std::pair<T1, T2>(
          Arbitrary<T1>::generate(generation),
          Arbitrary<T2>::generate(generation));
    }

    static std::vector<std::pair<T1, T2>> shrink(const std::pair<T1, T2>& t)
    {
      std::vector<T1> v1 = Arbitrary<T1>::shrink(t.first);
      std::vector<T2> v2 = Arbitrary<T2>::shrink(t.second);
      std::vector<std::pair<T1, T2>> v;
      auto it1 = v1.begin();
      auto it2 = v2.begin();
      for (; it1 != v1.end() && it2 != v2.end(); ++it1, ++it2)
        v.push_back(std::pair<T1, T2>(*it1, *it2));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specializations for map types
  //------------------------------------------------------------------------------
  template <typename K, typename V>
  struct Arbitrary<std::map<K, V>> : public Arbitrary_Assoc<std::map<K, V>> {};

  template <typename K, typename V>
  struct Arbitrary<std::multimap<K, V>> : public Arbitrary_Assoc<std::multimap<K, V>> {};

  template <typename K, typename V>
  struct Arbitrary<std::unordered_map<K, V>> : public Arbitrary_Assoc<std::unordered_map<K, V>> {};

}
