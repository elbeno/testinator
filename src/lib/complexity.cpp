#include <complexity.h>
using namespace testpp;

#include <algorithm>
#include <cmath>

//------------------------------------------------------------------------------
static const char* s_order[NUM_ORDERS] =
{
  "O(1)",
  "O(log N)",
  "O(N)",
  "O(N log N)",
  "O(N squared)",
};

const char* testpp::ComplexityProperty::Order(int o)
{
  return s_order[o];
}

int testpp::ComplexityProperty::CalculateOrder(
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
                 [actualRatio] (double d) { return std::abs(actualRatio - d); });
  auto m = std::min_element(ratio, &ratio[NUM_ORDERS]);
  return m - ratio;
}
