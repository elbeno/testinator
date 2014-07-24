#include <test_extended.h>
#include <property.h>
#include <algorithm>
#include <string>
using namespace std;

//------------------------------------------------------------------------------
DECLARE_TEST(Lambda, Property)
{
  testpp::Property<int> p([] (int) { return true; });
  return p.check();
}

//------------------------------------------------------------------------------
bool TestFunction(int)
{
  return true;
}

DECLARE_TEST(Function, Property)
{
  testpp::Property<int> p(TestFunction);
  return p.check();
}

//------------------------------------------------------------------------------
struct TestFunctor
{
  bool operator()(int) const { return true; }
};

DECLARE_TEST(Functor, Property)
{
  TestFunctor f;
  testpp::Property<int> p(f);
  return p.check();
}

//------------------------------------------------------------------------------
DECLARE_TEST(NullProperty, Property)
{
  testpp::Property<int> p([] (int) { return false; });
  return !p.check(1, true);
}

//------------------------------------------------------------------------------
DECLARE_TEST(StringReverse, Property)
{
  testpp::Property<string> p([] (const string& s) {
      string r(s);
      reverse(r.begin(), r.end());
      reverse(r.begin(), r.end());
      return s == r; });
  return p.check();
}
