#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>

namespace testpp
{

  enum
  {
    ORDER_1,
    ORDER_LOG_N,
    ORDER_N,
    ORDER_N_LOG_N,
    ORDER_N2,
    NUM_ORDERS
  };

  //------------------------------------------------------------------------------
  class ComplexityProperty
  {
    static const size_t MULTIPLIER = 32;
    static const size_t NUM_ITER = 5;

    static int CalculateOrder(
        unsigned long long* timesN, unsigned long long* timesMultN, size_t size,
        size_t N, size_t k);

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

    int check(std::size_t N = 100)
    {
      return m_internal->check(N);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual int check(std::size_t N) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      typedef function_traits<U> traits;
      typedef typename std::remove_cv<
        typename std::remove_reference<
          typename traits::argType>::type>::type paramType;

      Internal(const U& u) : m_u(u) {}

      virtual int check(std::size_t N)
      {
        // Get the timings for N and N * MULTIPLIER, NUM_ITER samples each
        unsigned long long countsN[NUM_ITER];
        unsigned long long countsMultN[NUM_ITER];
        for (std::size_t i = 0; i < NUM_ITER; ++i)
        {
          countsN[i] = checkInternal(N, N);
          countsMultN[i] = checkInternal(N, N * MULTIPLIER);
        }

        return CalculateOrder(countsN, countsMultN, NUM_ITER, N, MULTIPLIER);
      }

      unsigned long long checkInternal(std::size_t num, std::size_t N)
      {
        paramType p = Arbitrary<paramType>::generate_n(N);
        auto t1 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < num; ++i)
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
      m_numChecks = params.m_numPropertyChecks;                         \
      m_quiet = (params.m_flags & testpp::QUIET_SUCCESS) != 0;          \
      return true;                                                      \
    }                                                                   \
    virtual bool Run()                                                  \
    {                                                                   \
      testpp::ComplexityProperty p(*this);                              \
      int order = p.check(m_numChecks);                                 \
      bool success = (order <= testpp::ORDER);                          \
      if (!m_quiet || !success)                                         \
      {                                                                 \
        std::cout << m_name << ": ";                                    \
        std::cout << "expected "                                        \
                  << testpp::ComplexityProperty::Order(testpp::ORDER)   \
                  << ", actually "                                      \
                  << testpp::ComplexityProperty::Order(order) << std::endl; \
      }                                                                 \
      return success;                                                   \
    }                                                                   \
    void operator()(ARG) const;                                         \
    size_t m_numChecks;                                                 \
    bool m_quiet;                                                       \
  } s_##SUITE##NAME##_ComplexityProperty;                               \
  void SUITE##NAME##ComplexityProperty::operator()(ARG) const
