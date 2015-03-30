#pragma once

#include "capture-et.h"

#include <sstream>
#include <string>
#include <utility>

namespace testpp
{
#pragma once

  // -----------------------------------------------------------------------------
  // List template
  template <typename List>
  struct Cons
  {
    using type = List;
    List m_list;
  };

  struct Nil {};

  // -----------------------------------------------------------------------------
  // Output operator for List
  template <typename A, typename B>
  Cons<std::pair<A,B>> operator<<(Cons<A>&& a, B&& b) noexcept
  {
    return {{ std::forward<A>(a.m_list), std::forward<B>(b) }};
  }

  // Output operator for stream manipulators
  using ManipT = std::ostream& (*)(std::ostream&);

  template <typename A>
  Cons<std::pair<A,ManipT>> operator<<(Cons<A>&& a, ManipT m) noexcept
  {
    return {{ std::forward<A>(a.m_list), m }};
  }

  // Overload for string literals
  template <typename A, size_t N>
  Cons<std::pair<A,const char*>> operator<<(Cons<A>&& a, const char (&s)[N]) noexcept
  {
    return {{ std::forward<A>(a.m_list), s }};
  }

  // -----------------------------------------------------------------------------
  // Recursive log
  template <typename PairT>
  void LogR(std::ostream& os, PairT&& p) noexcept
  {
    LogR(os, std::forward<typename PairT::first_type>(p.first));
    os << std::forward<typename PairT::second_type>(p.second);
  }

  // Base case of recursion: at end of list, do nothing
  inline void LogR(std::ostream&, Nil) noexcept
  {
  }

  // -----------------------------------------------------------------------------
  // Stringify a diagnostic
  template <typename ConsT>
  __attribute__((__noinline__))
  std::string Diagnostic(ConsT&& t) noexcept
  {
    std::ostringstream s;
    s << std::boolalpha;
    LogR(s, std::forward<typename ConsT::type>(t.m_list));
    return s.str();
  }
}

// -----------------------------------------------------------------------------
#define DIAGNOSTIC(msg)                                       \
  (m_op->diagnostic(testpp::Diagnostic(                       \
                        testpp::Cons<testpp::Nil>() << msg)))

// -----------------------------------------------------------------------------
#define EXPECT(x)                                                       \
  {                                                                     \
    auto cap = testpp::Capture<testpp::Nothing>() ->* x;                \
    if (!testpp::Eval(cap))                                             \
    {                                                                   \
      auto diag =                                                       \
        testpp::Cons<testpp::Nil>() << "EXPECT FAILED: " __FILE__ ":"   \
                                    << __LINE__                         \
                                    << " (" #x " => " << LHS(cap);      \
      if (RelOp(cap)[0] != 0)                                           \
      {                                                                 \
        m_op->diagnostic(testpp::Diagnostic(                            \
                             std::move(diag) << " " << RelOp(cap)       \
                             << " " << RHS(cap) << ")"));               \
      }                                                                 \
      else                                                              \
      {                                                                 \
        m_op->diagnostic(testpp::Diagnostic(std::move(diag) << ")"));   \
      }                                                                 \
      m_success = false;                                                \
    }                                                                   \
  }
