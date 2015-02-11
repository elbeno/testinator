#pragma once

#include <chrono>
#include <iostream>

namespace testpp
{

  //------------------------------------------------------------------------------
  class TimedTest
  {
  public:
    template <typename F>
    TimedTest(const F& f)
    {
      m_internal = new Internal<F>(f);
    }

    ~TimedTest()
    {
      delete m_internal;
    }

    void check(std::size_t N = 100, bool quiet = true)
    {
      m_internal->check(N, quiet);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual void check(std::size_t N, bool quiet) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      Internal(const U& u) : m_u(u) {}

      virtual void check(std::size_t N, bool quiet)
      {
        auto t1 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < N; ++i)
        {
          m_u();
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        auto n = decltype(nanos)(N);

        if (!quiet)
        {
          std::cout << n << " tests run in " << nanos << "ns ("
                    << (nanos/n) << " ns per test)." << std::endl;
        }
      }

      U m_u;
    };

    InternalBase* m_internal;
  };

}

#include "test.h"

//------------------------------------------------------------------------------
#define DECLARE_TIMED_TEST(NAME, SUITE)                    \
  class SUITE##NAME##TimedTest : public testpp::Test       \
  {                                                        \
  public:                                                  \
    SUITE##NAME##TimedTest()                               \
      : testpp::Test(#NAME, #SUITE)                        \
      , m_numChecks(1)                                     \
    {}                                                     \
    virtual bool Setup(const testpp::RunParams& params)    \
    {                                                      \
      m_numChecks = params.m_numPropertyChecks;            \
      m_quiet = (params.m_flags & testpp::QUIET_SUCCESS) != 0;  \
      return true;                                         \
    }                                                      \
    virtual bool Run()                                     \
    {                                                      \
      testpp::TimedTest p(*this);                          \
      if (!m_quiet)                                        \
        std::cout << m_name << ": ";                       \
      p.check(m_numChecks, m_quiet);                       \
      return true;                                         \
    }                                                      \
    void operator()() const;                               \
    size_t m_numChecks;                                    \
    bool m_quiet;                                          \
  } s_##SUITE##NAME##_TimedTest;                           \
  void SUITE##NAME##TimedTest::operator()() const
