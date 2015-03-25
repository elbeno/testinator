#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>

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
        int64_t* countsN, int64_t* countsMultN, size_t size,
        size_t N, size_t k)
    {
      // sort the timings
      std::sort(&countsN[0], &countsN[size]);
      std::sort(&countsMultN[0], &countsMultN[size]);

      // discard the outer values, take the means of each set
      double timeN = static_cast<double>(
          std::accumulate(&countsN[1], &countsN[size-2], 0)) / (size - 2);
      double timeMultN = static_cast<double>(
          std::accumulate(&countsMultN[1], &countsMultN[size-2], 0)) / (size - 2);

      // the actual ratio of times
      double actualRatio = timeMultN / timeN;

      // calculate expected ratios
      double ratio[NUM_ORDERS] =
        {
          // O(1) =>
          // t(N) = K
          // t(kN) = K
          // t(kN) / t(N) = 1
          1.0,
          // O(log N) =>
          // t(N) = log(N)
          // t(kN) = log(kN)
          // t(kN) / t(N) = log(kN) / log(N) = (log(k) + log(N)) / log(N) = 1 + log(k) / log(N)
          1 + log2(k) / log2(N),
          // O(N) =>
          // t(N) = N
          // t(kN) = kN
          // t(kN) / t(N) = k
          static_cast<double>(k),
          // O(N log N) =>
          // t(N) = N * log(N)
          // t(kN) = kN * log(kN)
          // t(kN) / t(N) = kN * log(kN) / N * log(N) = k * log(kN) / log(N)
          k * (1 + log2(k) / log2(N)),
          // O(N^2) =>
          // t(N) = N^2
          // t(kN) = (kN)^2 = k^2 * N^2
          // t(kN) / t(N) = k^2 * N^2 / N^2 = k^2
          static_cast<double>(k * k)
        };

      // find the closest order to the to actual value
      std::transform(ratio, &ratio[NUM_ORDERS],
                     ratio,
                     [actualRatio] (double d)
                     { return std::abs(actualRatio - d); });
      auto m = std::min_element(ratio, &ratio[NUM_ORDERS]);
      return m - ratio;
    }

  public:
    static const char* Order(int o)
    {
      static const char* s_order[NUM_ORDERS] =
        {
          "O(1)",
          "O(log N)",
          "O(N)",
          "O(N log N)",
          "O(N squared)",
        };
      return s_order[o];
    }

    template <typename F>
    ComplexityProperty(const F& f)
      : m_internal(std::make_unique<Internal<F>>(f))
    {
    }

    int check(std::size_t N, unsigned long int randomSeed)
    {
      return m_internal->check(N, randomSeed);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual int check(std::size_t N, unsigned long int randomSeed) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      using paramType = std::decay_t<typename function_traits<U>::argType>;

      Internal(const U& u) : m_u(u) {}

      virtual int check(std::size_t N, unsigned long int randomSeed)
      {
        // Get the timings for N and N * MULTIPLIER, NUM_ITER samples each
        int64_t countsN[NUM_ITER];
        int64_t countsMultN[NUM_ITER];
        for (std::size_t i = 0; i < NUM_ITER; ++i)
        {
          countsN[i] = checkInternal(N, N, randomSeed);
          countsMultN[i] = checkInternal(N, N * MULTIPLIER, randomSeed);
        }

        return CalculateOrder(countsN, countsMultN, NUM_ITER, N, MULTIPLIER);
      }

      int64_t checkInternal(
          std::size_t num, std::size_t N, unsigned long int randomSeed)
      {
        paramType p = Arbitrary<paramType>::generate_n(N, randomSeed);
        m_u(p); // warm the cache
        auto t1 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < num; ++i)
        {
          m_u(p);
        }
        auto t2 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>
          (t2 - t1).count();
      }

      U m_u;
    };

    std::unique_ptr<InternalBase> m_internal;
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
      m_randomSeed = params.m_randomSeed;                               \
      if (m_randomSeed == 0)                                            \
      {                                                                 \
        std::random_device rd;                                          \
        m_randomSeed = rd();                                            \
      }                                                                 \
      return true;                                                      \
    }                                                                   \
    virtual bool Run()                                                  \
    {                                                                   \
      testpp::ComplexityProperty p(*this);                              \
      int order = p.check(m_numChecks, m_randomSeed);                   \
      bool success = (order <= testpp::ORDER);                          \
      if (!m_quiet || !success)                                         \
      {                                                                 \
        std::cout << m_name << ": ";                                    \
        std::cout << "expected "                                        \
                  << testpp::ComplexityProperty::Order(testpp::ORDER)   \
                  << ", actually "                                      \
                  << testpp::ComplexityProperty::Order(order);          \
        std::cout << " (seed=" << m_randomSeed << ")" << std::endl;     \
      }                                                                 \
      return success;                                                   \
    }                                                                   \
    void operator()(ARG) const;                                         \
    size_t m_numChecks;                                                 \
    bool m_quiet;                                                       \
    unsigned long int m_randomSeed;                                     \
  } s_##SUITE##NAME##_ComplexityProperty;                               \
  void SUITE##NAME##ComplexityProperty::operator()(ARG) const
