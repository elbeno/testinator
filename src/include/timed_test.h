// Copyright (c) 2014-2016 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "output.h"
#include "test_macros.h"

#include <chrono>
#include <memory>
#include <sstream>

namespace testinator
{

  //------------------------------------------------------------------------------
  class TimedTest
  {
  public:
    template <typename F>
    TimedTest(const F& f)
      : m_internal(std::make_unique<Internal<F>>(f))
    {
    }

    void check(std::size_t N, const Outputter* outputter)
    {
      m_internal->check(N, outputter);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual void check(std::size_t N, const Outputter*) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      Internal(const U& u) : m_u(u) {}

      virtual void check(std::size_t N,
                         const Outputter* op)
      {
        auto t1 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < N; ++i)
        {
          m_u();
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>
          (t2 - t1).count();
        auto n = decltype(nanos)(N);

        op->diagnostic(
            testinator::Diagnostic(
                testinator::Cons<testinator::Nil>()
                << m_u.GetName() << ": " << n << " tests run in " << nanos << "ns ("
                << (nanos/n) << " ns per test)."));
      }

      U m_u;
    };

    std::unique_ptr<InternalBase> m_internal;
  };

}

#include "test.h"

//------------------------------------------------------------------------------
#define DEF_TIMED_TEST(NAME, SUITE)                        \
  class SUITE##NAME##TimedTest : public testinator::Test   \
  {                                                        \
  public:                                                  \
    SUITE##NAME##TimedTest()                               \
      : testinator::Test(#NAME, #SUITE)                    \
      , m_numChecks(1)                                     \
    {}                                                     \
    virtual bool Setup(const testinator::RunParams& params)\
    {                                                      \
      m_numChecks = params.m_numPropertyChecks;            \
      return true;                                         \
    }                                                      \
    virtual bool Run()                                     \
    {                                                      \
      testinator::TimedTest p(*this);                      \
      p.check(m_numChecks, m_op);                          \
      return true;                                         \
    }                                                      \
    void operator()();                                     \
    size_t m_numChecks;                                    \
  } s_##SUITE##NAME##_TimedTest;                           \
  void SUITE##NAME##TimedTest::operator()()
