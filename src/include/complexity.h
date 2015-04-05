#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include "property.h"
#include "test_macros.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>

namespace testinator
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

    int check(std::size_t N)
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
      using argTuple = typename function_traits<U>::argTuple;

      Internal(const U& u) : m_u(u) {}

      virtual int check(std::size_t N) override
      {
        // Get the timings for N and N * MULTIPLIER, NUM_ITER samples each
        int64_t countsN[NUM_ITER];
        int64_t countsMultN[NUM_ITER];
        for (std::size_t i = 0; i < NUM_ITER; ++i)
        {
          countsN[i] = checkInternal(N, N);
          countsMultN[i] = checkInternal(N, N * MULTIPLIER);
        }

        return CalculateOrder(countsN, countsMultN, NUM_ITER, N, MULTIPLIER);
      }

      auto checkInternal(
          std::size_t num, std::size_t N)
      {
        auto seed = GetTestRegistry().RNG()();
        auto t = Arbitrary<argTuple>::generate_n(N, seed);
        auto d = function_traits<U>::apply_timed(num, m_u, t);
        return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
      }

      U m_u;
    };

    std::unique_ptr<InternalBase> m_internal;
  };

}

//------------------------------------------------------------------------------
#define DEF_COMPLEXITY_PROPERTY(NAME, SUITE, ORDER, ...)                \
  class SUITE##NAME##ComplexityProperty : public testinator::PropertyTest \
  {                                                                     \
  public:                                                               \
    SUITE##NAME##ComplexityProperty()                                   \
      : testinator::PropertyTest(#NAME "ComplexityProperty", #SUITE) {} \
    virtual bool Run() override                                         \
    {                                                                   \
      testinator::ComplexityProperty p(*this);                          \
      int order = p.check(m_numChecks);                                 \
      bool success = (order <= testinator::ORDER);                      \
      if (!success)                                                     \
      {                                                                 \
        m_op->diagnostic(                                               \
            testinator::Diagnostic(                                     \
                testinator::Cons<testinator::Nil>()                     \
                << m_name << ": expected "                              \
                << testinator::ComplexityProperty::Order(testinator::ORDER) \
                << ", actually "                                        \
                << testinator::ComplexityProperty::Order(order)));      \
      }                                                                 \
      return success;                                                   \
    }                                                                   \
    void operator()(__VA_ARGS__) const;                                 \
  } s_##SUITE##NAME##_ComplexityProperty;                               \
  void SUITE##NAME##ComplexityProperty::operator()(__VA_ARGS__) const
