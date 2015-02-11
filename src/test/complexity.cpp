#include <complexity.h>

#include <algorithm>
#include <string>
using namespace std;

DECLARE_COMPLEXITY_PROPERTY(O_1, Complexity, const string&, ORDER_1)
{
}

DECLARE_COMPLEXITY_PROPERTY(O_LOG_N, Complexity, string& s, ORDER_LOG_N)
{
  for (size_t i = 1, lim = s.size(); i < lim; i <<= 1)
  {
    s[i] = 'A';
  }
}

DECLARE_COMPLEXITY_PROPERTY(O_N, Complexity, const string& s, ORDER_N)
{
  max_element(s.begin(), s.end());
}

DECLARE_COMPLEXITY_PROPERTY(O_N_LOG_N, Complexity, const string& s, ORDER_N_LOG_N)
{
  for (size_t i = 1, lim = s.size(); i < lim; i <<= 1)
  {
    max_element(s.begin(), s.end());
  }
}
