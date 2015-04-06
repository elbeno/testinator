#pragma once

#include "arbitrary.h"

#include <tuple>
#include <utility>

namespace testinator
{
  namespace
  {
    inline auto nextRandom(unsigned long int randomSeed)
    {
      std::mt19937& gen = testinator::GetTestRegistry().RNG();
      gen.seed(randomSeed);
      std::uniform_int_distribution<unsigned long int> dis{};
      return dis(gen);
    }
  }

  //------------------------------------------------------------------------------
  // specialization for pair
  //------------------------------------------------------------------------------
  template <typename T1, typename T2>
  struct Arbitrary<std::pair<T1, T2>>
  {
    static std::pair<T1, T2> generate(std::size_t generation, unsigned long int randomSeed)
    {
      auto r1 = randomSeed;
      auto r2 = nextRandom(r1);

      return std::pair<T1, T2>(
          Arbitrary<T1>::generate(generation, r1),
          Arbitrary<T2>::generate(generation, r2));
    }

    static std::pair<T1, T2> generate_n(std::size_t n, unsigned long int randomSeed)
    {
      auto r1 = randomSeed;
      auto r2 = nextRandom(r1);

      return std::pair<T1, T2>(
          Arbitrary<T1>::generate_n(n, r1),
          Arbitrary<T2>::generate_n(n, r2));
    }

    static std::vector<std::pair<T1, T2>> shrink(const std::pair<T1, T2>& p)
    {
      std::vector<std::pair<T1, T2>> ret{};

      // shrink the first
      auto first_v = Arbitrary<T1>::shrink(p.first);
      for (T1& e : first_v)
      {
        ret.push_back(std::make_pair(std::move(e), p.second));
      }

      // shrink the second
      auto second_v = Arbitrary<T2>::shrink(p.second);
      for (T2& e : second_v)
      {
        ret.push_back(std::make_pair(p.first, std::move(e)));
      }

      return ret;
    }
  };

  //------------------------------------------------------------------------------
  // specialization for tuple
  //------------------------------------------------------------------------------
  template <typename T>
  struct is_tuple : public std::false_type {};
  template <typename... Ts>
  struct is_tuple<std::tuple<Ts...>> : public std::true_type {};

  template <typename T, std::size_t ...Is>
  auto tuple_tail(T&& t, std::index_sequence<Is...>,
                  std::enable_if_t<is_tuple<std::decay_t<T>>::value>* = nullptr)
  {
    return std::make_tuple(std::get<Is + 1>(std::forward<T>(t))...);
  }

  template <typename T>
  auto tuple_tail(T&& t, std::enable_if_t<is_tuple<std::decay_t<T>>::value>* = nullptr)
  {
    using Tuple = std::decay_t<T>;
    return tuple_tail(std::forward<T>(t),
                      std::make_index_sequence<std::tuple_size<Tuple>::value - 1>());
  }

  template <typename U, typename T, std::size_t ...Is>
  auto tuple_cons(U&& u, T&& t, std::index_sequence<Is...>,
                  std::enable_if_t<is_tuple<std::decay_t<T>>::value>* = nullptr)
  {
    return std::make_tuple(std::forward<U>(u), std::get<Is>(std::forward<T>(t))...);
  }

  template <typename U, typename T>
  auto tuple_cons(U&& u, T&& t,
                  std::enable_if_t<is_tuple<std::decay_t<T>>::value>* = nullptr)
  {
    using Tuple = std::decay_t<T>;
    return tuple_cons(std::forward<U>(u), std::forward<T>(t),
                      std::make_index_sequence<std::tuple_size<Tuple>::value>());
  }

  template <typename ...Ts>
  struct Arbitrary<std::tuple<Ts...>>
  {
    static std::tuple<Ts...> generate(std::size_t n, unsigned long int randomSeed)
    {
      auto r1 = randomSeed;
      auto r2 = nextRandom(r1);

      using H = std::tuple_element_t<0, std::tuple<Ts...>>;
      using T = decltype(tuple_tail(std::declval<std::tuple<Ts...>>()));
      return tuple_cons(Arbitrary<H>::generate(n, r1),
                        Arbitrary<T>::generate(n, r2));
    }

    static std::tuple<Ts...> generate_n(std::size_t n, unsigned long int randomSeed)
    {
      auto r1 = randomSeed;
      auto r2 = nextRandom(r1);

      using H = std::tuple_element_t<0, std::tuple<Ts...>>;
      using T = decltype(tuple_tail(std::declval<std::tuple<Ts...>>()));
      return tuple_cons(Arbitrary<H>::generate_n(n, r1),
                        Arbitrary<T>::generate_n(n, r2));
    }

    static std::vector<std::tuple<Ts...>> shrink(const std::tuple<Ts...>& t)
    {
      std::vector<std::tuple<Ts...>> ret{};

      // shrink the head
      auto head_v = Arbitrary<std::decay_t<decltype(std::get<0>(t))>>::shrink(std::get<0>(t));
      for (auto&& e : head_v)
      {
        ret.push_back(tuple_cons(std::forward<decltype(e)>(e), tuple_tail(t)));
      }

      // shrink the tail recursively
      auto tail_v = Arbitrary<std::decay_t<decltype(tuple_tail(t))>>::shrink(tuple_tail(t));
      for_each(tail_v.begin(), tail_v.end(),
               [&ret, &t] (auto&& v)
               {
                 ret.push_back(tuple_cons(std::get<0>(t), std::forward<decltype(v)>(v)));
               });

      return ret;
    }
  };

  template <>
  struct Arbitrary<std::tuple<>>
  {
    static std::tuple<> generate(std::size_t, unsigned long int)
    {
      return std::make_tuple();
    }

    static std::tuple<> generate_n(std::size_t, unsigned long int)
    {
      return std::make_tuple();
    }

    static std::vector<std::tuple<>> shrink(const std::tuple<>&)
    {
      return std::vector<std::tuple<>>{};
    }
  };

}
