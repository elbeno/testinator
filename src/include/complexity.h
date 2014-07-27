#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include <algorithm>
#include <chrono>
#include <iostream>

namespace testpp
{

  enum
  {
    ORDER_1,
    ORDER_LOG_N,
    ORDER_N,
    ORDER_N_LOG_N,
    ORDER_N_2
  };

  //------------------------------------------------------------------------------
  class ComplexityProperty
  {
    static const size_t N = 100;
    static const size_t COMPMULTLOG = 5;

  public:
    static const char* Order(int o);

    template <typename F>
    ComplexityProperty(const F& f)
    {
      m_internal = new Internal<F>(f);
    }

    ~ComplexityProperty()
    {
      delete m_internal;
    }

    int check(bool quiet = true, int expected_order = ORDER_N)
    {
      return m_internal->check(quiet, expected_order);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual int check(bool quiet, int expected_order) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      typedef function_traits<U> traits;
      typedef typename std::remove_cv<
        typename std::remove_reference<
          typename traits::argType>::type>::type paramType;

      Internal(const U& u) : m_u(u) {}

      virtual int check(bool quiet, int expected_order)
      {
        // vary the actual N by the expected order: we need to do more tests to
        // discern O(1) from O(log N) than we do to discern O(n log n) from O(n
        // squared)
        size_t n = N;// * (1 << ((ORDER_N_2 + 1) - expected_order));

        // Get the timings for N and (1<<COMPMULTLOG) * N, 5 samples each
        unsigned long long countsN[5];
        unsigned long long countsMultN[5];
        for (int i = 0; i < 5; ++i)
        {
          countsN[i] = checkInternal(n);
          countsMultN[i] = checkInternal(n * (1 << COMPMULTLOG));
        }

        // sort the timings
        std::sort(&countsN[0], &countsN[5]);
        std::sort(&countsMultN[0], &countsMultN[5]);

        // take the means of the 3 middle values of each set
        unsigned long long meanN = (countsN[1] + countsN[2] + countsN[3]) / 3;
        unsigned long long meanMultN = (countsMultN[1] + countsMultN[2] + countsMultN[3]) / 3;

        // now divide through and bucket into a complexity category
        double cat = (double)(meanMultN / meanN) / (double)(1 << COMPMULTLOG);

        // some notional values for each category
        const double O_1 = 1;
        const double O_log_n = COMPMULTLOG;
        const double O_n = (1<<COMPMULTLOG);
        const double O_n_log_n = (1<<COMPMULTLOG) * COMPMULTLOG;
        const double O_n_2 = COMPMULTLOG * COMPMULTLOG;

        // return the closest category: within 2x
        if (cat < O_1 || (cat - O_1) < O_1)
          return ORDER_1;
        if (cat < O_log_n || (cat - O_log_n) < O_log_n)
          return ORDER_LOG_N;
        if (cat < O_n || (cat - O_n) < O_n)
          return ORDER_N;
        if (cat < O_n_log_n || (cat - O_n_log_n) < O_n_log_n)
          return ORDER_N_LOG_N;
        return ORDER_N_2;
      }

      unsigned long long checkInternal(std::size_t N)
      {
        paramType p = Arbitrary<paramType>::generate(N);
        auto t1 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < N; ++i)
        {
          m_u(p);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        return (t2 - t1).count();
      }

      U m_u;
    };

    InternalBase* m_internal;
  };

}

#include "test.h"

//------------------------------------------------------------------------------
#define DECLARE_COMPLEXITY_PROPERTY(NAME, SUITE, ARG, ORDER)            \
  class SUITE##NAME##ComplexityProperty : public testpp::Test           \
  {                                                                     \
  public:                                                               \
    SUITE##NAME##ComplexityProperty()                                   \
      : testpp::Test(#NAME "ComplexityProperty", #SUITE)                \
    {}                                                                  \
    virtual bool Setup(const testpp::RunParams& params)                 \
    {                                                                   \
      m_quiet = (params.m_flags & testpp::QUIET_SUCCESS) != 0;          \
      return true;                                                      \
    }                                                                   \
    virtual bool Run()                                                  \
    {                                                                   \
      testpp::ComplexityProperty p(*this);                              \
      int order = p.check(m_quiet, testpp::ORDER);                      \
      bool exact = (order == testpp::ORDER);                            \
      if (!m_quiet || !exact)                                           \
      {                                                                 \
        std::cout << m_name << ": ";                                    \
        std::cout << "expected "                                        \
                  << testpp::ComplexityProperty::Order(testpp::ORDER)   \
                  << ", actually "                                      \
                  << testpp::ComplexityProperty::Order(order) << std::endl; \
      }                                                                 \
      return order <= testpp::ORDER;                                    \
    }                                                                   \
    void operator()(ARG) const;                                         \
    bool m_quiet;                                                       \
  } s_##SUITE##NAME##_ComplexityProperty;                               \
  void SUITE##NAME##ComplexityProperty::operator()(ARG) const
