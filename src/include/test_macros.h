// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "capture-et.h"
#include "prettyprint.h"

#include <sstream>
#include <string>
#include <utility>

#ifdef _MSC_VER
#define ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define ATTRIBUTE_NOINLINE __attribute__((__noinline__))
#endif

namespace testinator
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
  ATTRIBUTE_NOINLINE
  std::string Diagnostic(ConsT&& t) noexcept
  {
    std::ostringstream s;
    LogR(s, std::forward<typename ConsT::type>(t.m_list));
    return s.str();
  }
}

#undef ATTRIBUTE_NOINLINE

// -----------------------------------------------------------------------------
#define DIAGNOSTIC(msg)                                               \
  (m_op->diagnostic(testinator::Diagnostic(                           \
                        testinator::Cons<testinator::Nil>() << msg)))

#define ABORT(msg)                                                    \
  {                                                                   \
    m_op->abort(testinator::Diagnostic(                               \
                    testinator::Cons<testinator::Nil>() << msg));     \
    testinator::GetTestRegistry().Abort();                            \
  }

#define SKIP(msg)                                                     \
  {                                                                   \
    m_op->skipTest(GetName(), testinator::Diagnostic(                 \
                    testinator::Cons<testinator::Nil>() << msg));     \
    m_skipped = true;                                                 \
  }

// -----------------------------------------------------------------------------
#define EXPECT(x)                                                       \
  {                                                                     \
    auto cap = testinator::Capture<testinator::Nothing>() ->* x;        \
    if (!testinator::Eval(cap))                                         \
    {                                                                   \
      auto diag = testinator::Cons<testinator::Nil>()                   \
        << "EXPECT FAILED: " __FILE__ ":"  << __LINE__                  \
        << " (" #x " => " << testinator::prettyprint(LHS(cap));         \
      if (RelOp(cap)[0] != 0)                                           \
      {                                                                 \
        m_op->diagnostic(                                               \
            testinator::Diagnostic(                                     \
                std::move(diag) << " " << RelOp(cap) << " "             \
                << testinator::prettyprint(RHS(cap)) << ")"));          \
      }                                                                 \
      else                                                              \
      {                                                                 \
        m_op->diagnostic(testinator::Diagnostic(std::move(diag) << ")")); \
      }                                                                 \
      m_success = false;                                                \
    }                                                                   \
  }
