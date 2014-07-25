#include <test_extended.h>
#include <property.h>

#include <algorithm>
#include <string>
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
DECLARE_TEST(Lambda, Property)
{
  testpp::Property p([] (int) { return true; });
  return p.check();
}

//------------------------------------------------------------------------------
bool TestFunction(int)
{
  return true;
}

DECLARE_TEST(Function, Property)
{
  testpp::Property p(TestFunction);
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
  testpp::Property p(f);
  return p.check();
}

//------------------------------------------------------------------------------
DECLARE_TEST(NullProperty, Property)
{
  testpp::Property p([] (int) { return false; });
  return !p.check(1, true);
}

//------------------------------------------------------------------------------
DECLARE_TEST(StringReverse, Property)
{
  testpp::Property p([] (const string& s) {
      string r(s);
      reverse(r.begin(), r.end());
      reverse(r.begin(), r.end());
      return s == r; });
  return p.check();
}

//------------------------------------------------------------------------------
DECLARE_TEST(LambdaTraits, Property)
{
  auto lambda = [](int i) { return long(i*10); };

  typedef testpp::function_traits<decltype(lambda)> traits;

  return std::is_same<long, traits::result_type>::value
    && std::is_same<int, traits::arg<0>::type>::value;
}

//------------------------------------------------------------------------------
long funcTraitsFn(int);

DECLARE_TEST(FuncTraits, Property)
{
  typedef testpp::function_traits<decltype(funcTraitsFn)> traits;

  return std::is_same<long, traits::result_type>::value
    && std::is_same<int, traits::arg<0>::type>::value;
}

//------------------------------------------------------------------------------
struct FuncTraitsStruct
{
  long operator()(int);
};

DECLARE_TEST(FunctorTraits, Property)
{
  typedef testpp::function_traits<FuncTraitsStruct> traits;

  return std::is_same<long, traits::result_type>::value
    && std::is_same<int, traits::arg<0>::type>::value;
}

//------------------------------------------------------------------------------
DECLARE_PROPERTY(StringReverse, Property, const string& s)
{
  string r(s);
  reverse(r.begin(), r.end());
  reverse(r.begin(), r.end());
  return s == r;
}
