#pragma once

#include "arbitrary.h"
#include <algorithm>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

namespace testpp
{

  //------------------------------------------------------------------------------
  // specialization for vector
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::vector<T>, false>
  {
    static const std::size_t N = 10;

    static std::vector<T> generate(std::size_t generation = 0)
    {
      std::vector<T> v;
      std::generate_n(std::back_inserter(v), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::vector<T>> shrink(const std::vector<T>& t)
    {
      std::vector<std::vector<T>> v;
      if (t.size() < 2)
        return v;
      auto it = t.begin() + t.size()/2;
      v.push_back(std::vector<T>());
      copy(t.begin(), it, std::back_inserter(v[0]));
      v.push_back(std::vector<T>());
      copy(it, t.end(), std::back_inserter(v[1]));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for deque
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::deque<T>, false>
  {
    static const std::size_t N = 10;

    static std::deque<T> generate(std::size_t generation = 0)
    {
      std::deque<T> v;
      std::generate_n(std::back_inserter(v), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::deque<T>> shrink(const std::deque<T>& t)
    {
      std::vector<std::deque<T>> v;
      if (t.size() < 2)
        return v;
      auto it = t.begin() + t.size()/2;
      v.push_back(std::deque<T>());
      copy(t.begin(), it, std::back_inserter(v[0]));
      v.push_back(std::deque<T>());
      copy(it, t.end(), std::back_inserter(v[1]));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for list
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::list<T>, false>
  {
    static const std::size_t N = 10;

    static std::list<T> generate(std::size_t generation = 0)
    {
      std::list<T> v;
      std::generate_n(std::back_inserter(v), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::list<T>> shrink(const std::list<T>& t)
    {
      std::vector<std::list<T>> v;
      if (t.size() < 2)
        return v;
      auto it = t.begin();
      v.push_back(std::list<T>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].push_front(*it);
      v.push_back(std::list<T>());
      copy(it, t.end(), std::front_inserter(v[1]));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for forward_list
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::forward_list<T>, false>
  {
    static const std::size_t N = 10;

    static std::forward_list<T> generate(std::size_t generation = 0)
    {
      std::forward_list<T> v;
      std::generate_n(std::front_inserter(v), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::forward_list<T>> shrink(const std::forward_list<T>& t)
    {
      std::vector<std::forward_list<T>> v;
      std::size_t len = 0;
      for (auto it = t.begin(); it != t.end(); ++it, ++len);
      if (len < 2)
        return v;

      auto it = t.begin();
      v.push_back(std::forward_list<T>());
      for (int count = 0; count < len/2; count++, it++)
        v[0].push_front(*it);
      v.push_back(std::forward_list<T>());
      copy(it, t.end(), std::front_inserter(v[1]));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for set
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::set<T>, false>
  {
    static const std::size_t N = 10;

    static std::set<T> generate(std::size_t generation = 0)
    {
      std::set<T> v;
      std::generate_n(std::inserter(v, v.begin()), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::set<T>> shrink(const std::set<T>& t)
    {
      std::vector<std::set<T>> v;
      auto it = t.begin();
      v.push_back(std::set<T>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].insert(*it);
      v.push_back(std::set<T>());
      copy(it, t.end(), std::inserter(v[1], v[1].begin()));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for multiset
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::multiset<T>, false>
  {
    static const std::size_t N = 10;

    static std::multiset<T> generate(std::size_t generation = 0)
    {
      std::multiset<T> v;
      std::generate_n(std::inserter(v, v.begin()), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::multiset<T>> shrink(const std::multiset<T>& t)
    {
      std::vector<std::multiset<T>> v;
      auto it = t.begin();
      v.push_back(std::multiset<T>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].insert(*it);
      v.push_back(std::multiset<T>());
      copy(it, t.end(), std::inserter(v[1], v[1].begin()));
      return v;
    }
  };

 //------------------------------------------------------------------------------
  // specialization for unordered_set
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::unordered_set<T>, false>
  {
    static const std::size_t N = 10;

    static std::unordered_set<T> generate(std::size_t generation = 0)
    {
      std::unordered_set<T> v;
      std::generate_n(std::inserter(v, v.begin()), N * ((generation / 100) + 1),
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::vector<std::unordered_set<T>> shrink(const std::unordered_set<T>& t)
    {
      std::vector<std::unordered_set<T>> v;
      auto it = t.begin();
      v.push_back(std::unordered_set<T>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].insert(*it);
      v.push_back(std::unordered_set<T>());
      copy(it, t.end(), std::inserter(v[1], v[1].begin()));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for pair
  //------------------------------------------------------------------------------
  template <typename T1, typename T2>
  struct Arbitrary<std::pair<T1, T2>, false>
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
  // specialization for map
  //------------------------------------------------------------------------------
  template <typename K, typename V>
  struct Arbitrary<std::map<K, V>, false>
  {
    static const std::size_t N = 10;

    static std::map<K, V> generate(std::size_t generation = 0)
    {
      std::map<K, V> v;
      std::generate_n(
          std::inserter(v, v.begin()), N * ((generation / 100) + 1),
          [&] () { return Arbitrary<typename std::map<K, V>::value_type>::generate(
                generation++); });
      return v;
    }

    static std::vector<std::map<K, V>> shrink(const std::map<K, V>& t)
    {
      std::vector<std::map<K, V>> v;
      auto it = t.begin();
      v.push_back(std::map<K, V>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].insert(*it);
      v.push_back(std::map<K, V>());
      copy(it, t.end(), std::inserter(v[1], v[1].begin()));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for multimap
  //------------------------------------------------------------------------------
  template <typename K, typename V>
  struct Arbitrary<std::multimap<K, V>, false>
  {
    static const std::size_t N = 10;

    static std::multimap<K, V> generate(std::size_t generation = 0)
    {
      std::multimap<K, V> v;
      std::generate_n(
          std::inserter(v, v.begin()), N * ((generation / 100) + 1),
          [&] () { return Arbitrary<typename std::multimap<K, V>::value_type>::generate(
                generation++); });
      return v;
    }

    static std::vector<std::multimap<K, V>> shrink(const std::multimap<K, V>& t)
    {
      std::vector<std::multimap<K, V>> v;
      auto it = t.begin();
      v.push_back(std::multimap<K, V>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].insert(*it);
      v.push_back(std::multimap<K, V>());
      copy(it, t.end(), std::inserter(v[1], v[1].begin()));
      return v;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for unordered_map
  //------------------------------------------------------------------------------
  template <typename K, typename V>
  struct Arbitrary<std::unordered_map<K, V>, false>
  {
    static const std::size_t N = 10;

    static std::unordered_map<K, V> generate(std::size_t generation = 0)
    {
      std::unordered_map<K, V> v;
      std::generate_n(
          std::inserter(v, v.begin()), N * ((generation / 100) + 1),
          [&] () { return Arbitrary<typename std::unordered_map<K, V>::value_type>::generate(
                generation++); });
      return v;
    }

    static std::vector<std::unordered_map<K, V>> shrink(const std::unordered_map<K, V>& t)
    {
      std::vector<std::unordered_map<K, V>> v;
      auto it = t.begin();
      v.push_back(std::unordered_map<K, V>());
      for (int count = 0; count < t.size()/2; count++, it++)
        v[0].insert(*it);
      v.push_back(std::unordered_map<K, V>());
      copy(it, t.end(), std::inserter(v[1], v[1].begin()));
      return v;
    }
  };

}
