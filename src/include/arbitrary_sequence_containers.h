#pragma once

#include "arbitrary.h"
#include <algorithm>
#include <deque>
#include <forward_list>
#include <list>
#include <vector>

namespace testpp
{

  namespace
  {
    template <typename C>
    struct Arbitrary_RandomSequence
    {
      static const std::size_t N = 10;
      typedef typename C::value_type V;

      static C generate(std::size_t generation = 0)
      {
        C v;
        std::size_t n = N * ((generation / 100) + 1);
        std::generate_n(
            std::back_inserter(v), n,
            [&] () { return Arbitrary<V>::generate(generation++); });
        return v;
      }

      static C generate_n(std::size_t n)
      {
        C v;
        std::generate_n(
            std::back_inserter(v), n,
            [&] () { return Arbitrary<V>::generate(n); });
        return v;
      }

      static std::vector<C> shrink(const C& c)
      {
        std::vector<C> v;
        if (c.size() < 2)
          return v;
        auto it = c.begin() + c.size()/2;
        v.push_back(C());
        copy(c.begin(), it, std::back_inserter(v[0]));
        v.push_back(C());
        copy(it, c.end(), std::back_inserter(v[1]));
        return v;
      }
    };
  }

  //------------------------------------------------------------------------------
  // specializations for sequence containers
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::vector<T>> : public Arbitrary_RandomSequence<std::vector<T>> {};

  template <typename T>
  struct Arbitrary<std::deque<T>> : public Arbitrary_RandomSequence<std::deque<T>> {};

  //------------------------------------------------------------------------------
  // specialization for list
  //------------------------------------------------------------------------------
  template <typename T>
  struct Arbitrary<std::list<T>>
  {
    static const std::size_t N = 10;

    static std::list<T> generate(std::size_t generation = 0)
    {
      std::list<T> v;
      std::size_t n = N * ((generation / 100) + 1);
      std::generate_n(std::back_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::list<T> generate_n(std::size_t n)
    {
      std::list<T> v;
      std::generate_n(std::back_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate(n); });
      return v;
    }

    static std::vector<std::list<T>> shrink(const std::list<T>& t)
    {
      std::vector<std::list<T>> v;
      if (t.size() < 2)
        return v;
      auto it = t.begin();
      v.push_back(std::list<T>());
      for (std::size_t count = 0; count < t.size()/2; count++, it++)
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
  struct Arbitrary<std::forward_list<T>>
  {
    static const std::size_t N = 10;

    static std::forward_list<T> generate(std::size_t generation = 0)
    {
      std::forward_list<T> v;
      std::size_t n = N * ((generation / 100) + 1);
      std::generate_n(std::front_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate(generation++); });
      return v;
    }

    static std::forward_list<T> generate_n(std::size_t n)
    {
      std::forward_list<T> v;
      std::generate_n(std::front_inserter(v), n,
                      [&] () { return Arbitrary<T>::generate(n); });
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
      for (std::size_t count = 0; count < len/2; count++, it++)
        v[0].push_front(*it);
      v.push_back(std::forward_list<T>());
      copy(it, t.end(), std::front_inserter(v[1]));
      return v;
    }
  };

}
