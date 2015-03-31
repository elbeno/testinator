#include <timed_test.h>

#include <string>
using namespace std;

DEF_TIMED_TEST(Simple, Timed)
{
  string s = "Hello, world!";
  for (int i = 0; i < 100; ++i)
  {
    reverse(s.begin(), s.end());
  }
}
