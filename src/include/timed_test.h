#pragma once

#include "output.h"

#include <chrono>
#include <memory>
#include <sstream>

namespace testpp
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
        m_u(); // warm the cache
        auto t1 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < N; ++i)
        {
          m_u();
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>
          (t2 - t1).count();
        auto n = decltype(nanos)(N);

        std::ostringstream s;
        s << m_u.name() << ": " << n << " tests run in " << nanos << "ns ("
          << (nanos/n) << " ns per test).";
        op->diagnostic(s.str());
      }

      U m_u;
    };

    std::unique_ptr<InternalBase> m_internal;
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
      return true;                                         \
    }                                                      \
    virtual bool Run()                                     \
    {                                                      \
      testpp::TimedTest p(*this);                          \
      p.check(m_numChecks, m_op);                          \
      return true;                                         \
    }                                                      \
    void operator()() const;                               \
    size_t m_numChecks;                                    \
  } s_##SUITE##NAME##_TimedTest;                           \
  void SUITE##NAME##TimedTest::operator()() const
