// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace testinator
{

// -----------------------------------------------------------------------------
// A pretty-printer for (pretty much) any type.
//
// * If operator<< is available, it will be used.
// * If the type is not printable (like a union) we will attempt to
//   characterize it anyway using type_traits and print something like
//   <union>.
// * Containers (iterable, with begin() and end()) get printed with customizable
//   openers, closers and separators. The default is {}, with [] for vectors
//   and arrays, and comma for a separator.
// * Pairs are printed (like,this), as are tuples. This is also customizable in
//   the same way as containers.
// * Strings and char arrays are printed with surrounding quotes. Again,
//   customizable (if for example, you want single quotes).
// * Enum values and enum class values are printed as integral values.
// * Objects with operator() that can implicitly convert to bool are output as
//   <callable> even though operator<< would work. An example is non-capturing
//   lambdas, which can implicit convert to pointer-to-function and thus to
//   bool.
//
// To use, do something like:
// cout << prettyprint(x);
// or:
// cout << prettyprint(x, formatter);

// -----------------------------------------------------------------------------
// SFINAE member/functionality detection
#define SFINAE_DETECT(name, expr)                                       \
  template <typename T>                                                 \
  using name##_t = decltype(expr);                                      \
  template <typename T, typename = void>                                \
  struct has_##name : public std::false_type {};                        \
  template <typename T>                                                 \
  struct has_##name<T, detail::void_t<name##_t<T>>> : public std::true_type {};

namespace detail
{

  // ---------------------------------------------------------------------------
#ifdef __clang__
  template <typename...>
  using void_t = void;
#else
  template<class...> struct voider { using type = void; };
  template<class... Args> using void_t = typename voider<Args...>::type;
#endif

  // ---------------------------------------------------------------------------
  // Is the type iterable (has begin() and end())?
  SFINAE_DETECT(begin, std::declval<T>().begin())
  SFINAE_DETECT(end, std::declval<T>().end())

  template<typename T>
  using is_iterable = typename std::conditional<
    has_begin<T>::value && has_end<T>::value,
    std::true_type, std::false_type>::type;

  struct is_iterable_tag {};

  // ---------------------------------------------------------------------------
  // Is the type a pair or tuple?
  template <typename T>
  struct is_pair : public std::false_type {};
  template <typename T, typename U>
  struct is_pair<std::pair<T, U>> : public std::true_type {};

  struct is_pair_tag {};

  template <typename T>
  struct is_tuple : public std::false_type {};
  template <typename... Ts>
  struct is_tuple<std::tuple<Ts...>> : public std::true_type {};

  struct is_tuple_tag {};

  // ---------------------------------------------------------------------------
  // Is the type a callable of some kind?
  SFINAE_DETECT(call_operator, std::declval<T>()())

  template <typename T>
  struct is_std_function : public std::false_type {};
  template <typename T>
  struct is_std_function<std::function<T>> : public std::true_type {};

  template<typename T>
  using is_callable = typename std::conditional<
    has_call_operator<T>::value
    || is_std_function<T>::value
    || std::is_function<T>::value
    || std::is_bind_expression<T>::value,
    std::true_type, std::false_type>::type;

  struct is_callable_tag {};

  template <typename T>
  constexpr static std::enable_if_t<is_std_function<T>::value, const char*>
  callable_type() { return "(std::function)"; }

  template <typename T>
  constexpr static std::enable_if_t<std::is_function<T>::value, const char*>
  callable_type() { return "(function)"; }

  template <typename T>
  constexpr static
  std::enable_if_t<std::is_bind_expression<T>::value,
                   const char*>
  callable_type() { return "(bind expression)"; }

  template <typename T>
  constexpr static
  std::enable_if_t<
    !is_std_function<T>::value && !std::is_function<T>::value &&
    !std::is_bind_expression<T>::value && has_call_operator<T>::value,
    const char*>
    callable_type() { return "(function object)"; }

  // ---------------------------------------------------------------------------
  // Does the type support operator<< ?
  SFINAE_DETECT(operator_output, std::cout << std::declval<T>())

  // Non-capturing lambdas (and some other callables) may implicitly convert to
  // bool, which will make operator<< work. We want to treat them as callables,
  // not outputtables.
  void bool_conversion_test(bool);
  SFINAE_DETECT(bool_conversion, bool_conversion_test(std::declval<T>()))

  template<typename T>
  using is_outputtable = typename std::conditional<
    has_operator_output<T>::value &&
    !std::is_function<T>::value &&
    !(has_call_operator<T>::value && has_bool_conversion<T>::value),
    std::true_type, std::false_type>::type;

  struct is_outputtable_tag {};

  // ---------------------------------------------------------------------------
  // Is the type an enum or enum class?
  struct is_enum_tag {};

  // ---------------------------------------------------------------------------
  // Is the type something unprintable?
  template<typename T>
  using is_unprintable = typename std::conditional<
    std::is_union<T>::value ||
    std::is_class<T>::value ||
    std::is_null_pointer<T>::value,
    std::true_type, std::false_type>::type;

  struct is_unprintable_tag {};

  template <typename T>
  constexpr static std::enable_if_t<std::is_union<T>::value, const char*>
  unprintable_type() { return "<union>"; }

  template <typename T>
  constexpr static std::enable_if_t<std::is_class<T>::value, const char*>
  unprintable_type() { return "<class>"; }

  template <typename T>
  constexpr static
  std::enable_if_t<std::is_null_pointer<T>::value, const char*>
  unprintable_type() { return "<nullptr>"; }

  // The way we want to treat a type, in preference order.
  template <typename T>
  using stringifier_tag = std::conditional_t<
    std::is_enum<T>::value,
    is_enum_tag,
    std::conditional_t<
      is_outputtable<T>::value,
      is_outputtable_tag,
      std::conditional_t<
        is_callable<T>::value,
        is_callable_tag,
        std::conditional_t<
          is_iterable<T>::value,
          is_iterable_tag,
          std::conditional_t<
            is_pair<T>::value,
            is_pair_tag,
            std::conditional_t<
              is_tuple<T>::value,
              is_tuple_tag,
              std::conditional_t<
                is_unprintable<T>::value,
                is_unprintable_tag,
                void>>>>>>>;

} // detail

// -----------------------------------------------------------------------------
template <typename T, typename F, typename TAG>
struct stringifier_select;

template <typename T, typename F>
using stringifier = stringifier_select<T, F, detail::stringifier_tag<std::remove_cv_t<T>>>;

template <typename T, typename F>
inline std::ostream& operator<<(std::ostream& s, const stringifier<T, F>& t)
{
  return t.output(s);
}

// -----------------------------------------------------------------------------
// Customization points for printing containers, pairs, tuples, strings
struct default_formatter
{
  // default separator, opener and closer
  template <typename T>
  constexpr const char* separator(const T&) const
  { return ","; }

  template <typename T>
  constexpr const char* opener(const T&) const
  { return "{"; }

  template <typename T>
  constexpr const char* closer(const T&) const
  { return "}"; }

  // use [] for vectors and arrays
  template <typename T>
  constexpr const char* opener(const std::vector<T>&) const
  { return "["; }

  template <typename T>
  constexpr const char* closer(const std::vector<T>&) const
  { return "]"; }

  template <typename T, size_t N>
  constexpr const char* opener(const std::array<T, N>&) const
  { return "["; }

  template <typename T, size_t N>
  constexpr const char* closer(const std::array<T, N>&) const
  { return "]"; }

  template <typename T, size_t N>
  constexpr const char* opener(const T(&)[N]) const
  { return "["; }

  template <typename T, size_t N>
  constexpr const char* closer(const T(&)[N]) const
  { return "]"; }

  // use () for pairs and tuples
  template <typename T, typename U>
  constexpr const char* opener(const std::pair<T, U>&) const
  { return "("; }

  template <typename T, typename U>
  constexpr const char* closer(const std::pair<T, U>&) const
  { return ")"; }

  template <typename... Ts>
  constexpr const char* opener(const std::tuple<Ts...>&) const
  { return "("; }

  template <typename... Ts>
  constexpr const char* closer(const std::tuple<Ts...>&) const
  { return ")"; }

  // use double quotes for strings
  constexpr const char* opener(const std::string&) const
  { return "\""; }

  constexpr const char* closer(const std::string&) const
  { return "\""; }

  constexpr const char* opener(const char* const) const
  { return "\""; }

  constexpr const char* closer(const char* const) const
  { return "\""; }
};

// -----------------------------------------------------------------------------
// The function that drives it all
template <typename T>
inline stringifier<std::remove_reference_t<T>, default_formatter>
prettyprint(T&& t)
{
  return stringifier<std::remove_reference_t<T>, default_formatter>(
      std::forward<T>(t), default_formatter());
}

template <typename T, typename F>
inline stringifier<std::remove_reference_t<T>, F>
prettyprint(T&& t, F&& f)
{
  return stringifier<std::remove_reference_t<T>, F>(
      std::forward<T>(t), std::forward<F>(f));
}

// -----------------------------------------------------------------------------
// Default: not stringifiable
template <typename T, typename F, typename TAG>
struct stringifier_select
{
  explicit stringifier_select(const T&, F) {}

  std::ostream& output(std::ostream& s) const
  {
    return s << "<unknown>";
  }
};

// -----------------------------------------------------------------------------
// Specialization for outputtable
template <typename T, typename F>
struct stringifier_select<T, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(const T& t, F)
    : m_t(t) {}

  std::ostream& output(std::ostream& s) const
  {
    return s << m_t;
  }

  const T& m_t;
};

// -----------------------------------------------------------------------------
// Specialize for string
template <typename C, typename T, typename A, typename F>
struct stringifier_select<std::basic_string<C,T,A>, F, detail::is_outputtable_tag>
{
  using S = std::basic_string<C,T,A>;
  explicit stringifier_select(const S& t, const F& f)
    : m_t(t)
    , m_f(f)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return s << m_f.opener(m_t)
             << m_t
             << m_f.closer(m_t);
  }

  const S& m_t;
  const F& m_f;
};

template <typename C, typename T, typename A, typename F>
struct stringifier_select<const std::basic_string<C,T,A>, F, detail::is_outputtable_tag>
  : public stringifier_select<std::basic_string<C,T,A>, F, detail::is_outputtable_tag>
{
  using S = std::basic_string<C,T,A>;
  explicit stringifier_select(const S& t, const F& f)
    : stringifier_select<std::basic_string<C,T,A>, F, detail::is_outputtable_tag>(t, f)
  {}
};

// -----------------------------------------------------------------------------
// Specialize for char* pointer-to-const and const-pointer varieties
template <typename F>
struct stringifier_select<char*, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(const char* const t, const F& f)
    : m_t(t)
    , m_f(f)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return s << m_f.opener(m_t)
             << m_t
             << m_f.closer(m_t);
  }

  const char* const m_t;
  const F& m_f;
};

template <typename F>
struct stringifier_select<char* const, F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(char* const t, const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

template <typename F>
struct stringifier_select<const char*, F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(const char* t, const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

template <typename F>
struct stringifier_select<const char* const, F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(const char* const t, const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

// -----------------------------------------------------------------------------
// Specialize for const char[] and char[]
template <size_t N, typename F>
struct stringifier_select<char[N], F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  using S = char[N];
  explicit stringifier_select(const S& t, const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

template <size_t N, typename F>
struct stringifier_select<const char[N], F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  using S = char[N];
  explicit stringifier_select(const S& t, const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

template <typename F>
struct stringifier_select<char[], F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(char t[], const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

template <typename F>
struct stringifier_select<const char[], F, detail::is_outputtable_tag>
  : public stringifier_select<char*, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(const char t[], const F& f)
    : stringifier_select<char*, F, detail::is_outputtable_tag>(t, f)
  {}
};

// -----------------------------------------------------------------------------
// Specialize for arrays
namespace detail
{
  template <typename T, typename F>
  inline std::ostream& output_iterable(std::ostream& s,
                                       const T& t,
                                       const F& f)
  {
    s << f.opener(t);
    auto b = std::begin(t);
    auto e = std::end(t);
    if (b != e)
    {
      s << prettyprint(*b);
      std::for_each(++b, e,
                    [&s, &t, &f] (auto&& elem)
                    { s << f.separator(t)
                        << prettyprint(std::forward<decltype(elem)>(elem)); });
    }
    return s << f.closer(t);
  }
}

template <typename T, size_t N, typename F>
struct stringifier_select<T[N], F, detail::is_outputtable_tag>
{
  using S = T[N];
  explicit stringifier_select(const S& t, const F& f)
    : m_t(t)
    , m_f(f)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return detail::output_iterable<S, F>(s, m_t, m_f);
  }

  const S& m_t;
  const F& m_f;
};

template <typename T, typename F>
struct stringifier_select<T[], F, detail::is_outputtable_tag>
{
  explicit stringifier_select(T[], F)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return s << "<array (unknown bounds)>";
  }
};

// -----------------------------------------------------------------------------
// Specialize for bool: do boolalpha explicitly so as not to affect stream state
template <typename F>
struct stringifier_select<bool, F, detail::is_outputtable_tag>
{
  explicit stringifier_select(bool t, F)
    : m_t(t) {}

  std::ostream& output(std::ostream& s) const
  {
    return s << (m_t ? "true" : "false");
  }

  bool m_t;
};

// -----------------------------------------------------------------------------
// Specialize for iterable, with customization of opener/closer/separator
template <typename T, typename F>
struct stringifier_select<T, F, detail::is_iterable_tag>
{
  explicit stringifier_select(const T& t, const F& f)
    : m_t(t)
    , m_f(f)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return detail::output_iterable<std::decay_t<T>, F>(s, m_t, m_f);
  }

  const T& m_t;
  const F& m_f;
};

// -----------------------------------------------------------------------------
// Specialization for callable object
template <typename T, typename F>
struct stringifier_select<T, F, detail::is_callable_tag>
{
  explicit stringifier_select(T, F) {}
  std::ostream& output(std::ostream& s) const
  {
    return s << "<callable "
             << detail::callable_type<T>()
             << '>';
  }
};

// -----------------------------------------------------------------------------
// Specialization for unprintable object
template <typename T, typename F>
struct stringifier_select<T, F, detail::is_unprintable_tag>
{
  explicit stringifier_select(T, F) {}

  std::ostream& output(std::ostream& s) const
  {
    return s << detail::unprintable_type<T>();
  }
};

// -----------------------------------------------------------------------------
// Specialization for enum
template <typename T, typename F>
struct stringifier_select<T, F, detail::is_enum_tag>
{
  explicit stringifier_select(T t, F)
    : m_t(t)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return s << static_cast<std::underlying_type_t<T>>(m_t);
  }

  T m_t;
};

// -----------------------------------------------------------------------------
// Specialization for pair
template <typename T, typename F>
struct stringifier_select<T, F, detail::is_pair_tag>
{
  explicit stringifier_select(const T& t, const F& f)
    : m_t(t)
    , m_f(f)
  {}

  std::ostream& output(std::ostream& s) const
  {
    return s << m_f.opener(m_t)
             << prettyprint(m_t.first)
             << m_f.separator(m_t)
             << prettyprint(m_t.second)
             << m_f.closer(m_t);
  }

  const T& m_t;
  const F& m_f;
};

// -----------------------------------------------------------------------------
// Specialization for tuple
namespace detail
{
  template <typename F, typename...Ts, std::size_t...Is>
  inline void for_each_in_tuple(const std::tuple<Ts...>& t, F f,
                                std::index_sequence<Is...>)
  {
    using I = std::initializer_list<int>;
    (void) I { (f(std::get<Is>(t), Is), 0)... };
  }

  template <typename F, typename...Ts>
  inline void for_each_in_tuple(const std::tuple<Ts...>& t, F f)
  {
    for_each_in_tuple(t, f, std::make_index_sequence<sizeof...(Ts)>());
  }
} // detail

template <typename T, typename F>
struct stringifier_select<T, F, detail::is_tuple_tag>
{
  explicit stringifier_select(const T& t, const F& f)
    : m_t(t)
    , m_f(f)
  {}

  std::ostream& output(std::ostream& s) const
  {
    s << m_f.opener(m_t);
    detail::for_each_in_tuple(m_t,
                              [&s, this] (auto&& e, size_t i)
                              { if (i > 0) s << m_f.separator(m_t);
                                s << prettyprint(std::forward<decltype(e)>(e)); });
    return s << m_f.closer(m_t);
  }

  const T& m_t;
  const F& m_f;
};

#undef SFINAE_DETECT
}
