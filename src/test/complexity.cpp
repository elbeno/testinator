#include <complexity.h>

#include <algorithm>
#include <string>
using namespace std;

DEF_COMPLEXITY_PROPERTY(O_1, Complexity, ORDER_1, const string&)
{
}

DEF_COMPLEXITY_PROPERTY(O_LOG_N, Complexity, ORDER_LOG_N, string& s)
{
  for (size_t i = 1, lim = s.size(); i < lim; i <<= 1)
  {
    s[i] = 'A';
  }
}

DEF_COMPLEXITY_PROPERTY(O_N, Complexity, ORDER_N, const string& s)
{
  max_element(s.begin(), s.end());
}

DEF_COMPLEXITY_PROPERTY(O_N_LOG_N, Complexity, ORDER_N_LOG_N, const string& s)
{
  for (size_t i = 1, lim = s.size(); i < lim; i <<= 1)
  {
    max_element(s.begin(), s.end());
  }
}
