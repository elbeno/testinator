#include <property.h>

using namespace std;

//------------------------------------------------------------------------------
// A test of the machinery

struct TestFunctor
{
  bool operator()(int) const { return true; }
  unsigned long m_randomSeed = 0;
};

DECLARE_TEST(Functor, Property)
{
  TestFunctor f;
  testinator::Property p(f);
  testinator::Outputter op;
  return p.check(0, &op);
}

//------------------------------------------------------------------------------
// Another machinery test

struct FuncTraitsStruct
{
  void operator()(int);
};

DECLARE_TEST(FunctorTraits, Property)
{
  using traits = testinator::function_traits<FuncTraitsStruct>;

  return std::is_same<int, traits::argType>::value;
}

//------------------------------------------------------------------------------
// A simple test of string reversal invariant

DECLARE_PROPERTY(StringReverse, Property, const string& s)
{
  string r(s);
  reverse(r.begin(), r.end());
  reverse(r.begin(), r.end());
  return s == r;
}

//------------------------------------------------------------------------------
// This is supposed to fail to demonstrate how shrinking finds the minimal
// failure

DECLARE_PROPERTY(FailTriggersShrink, Property, const string& s)
{
  return s.find('A') == s.npos;
}

//------------------------------------------------------------------------------
// A user-defined type test

struct MyType
{
  MyType() : m_val(1337) {}
  int m_val;
};

ostream& operator<<(ostream& s, const MyType& m)
{
  return s << m.m_val;
}

DECLARE_PROPERTY(MyType, Property, const MyType& m)
{
  return m.m_val == 1337;
}

//------------------------------------------------------------------------------
DECLARE_PROPERTY(ConstInt, Property, const int)
{
  return true;
}

//------------------------------------------------------------------------------
DECLARE_PROPERTY(ConstChar, Property, const char)
{
  return true;
}

//------------------------------------------------------------------------------
// A user-defined type can also be used to produce a bounded type

struct MyBoundedType
{
  static const int MAX_VAL = 10;
  MyBoundedType(int v) : m_val(v) {}
  int m_val;
};

ostream& operator<<(ostream& s, const MyBoundedType& m)
{
  return s << "MyBoundedType(" << m.m_val << ")";
}

template <>
struct testinator::Arbitrary<MyBoundedType>
{
  static MyBoundedType generate(std::size_t generation, unsigned long int /*randomSeed*/)
  { return MyBoundedType(generation % MyBoundedType::MAX_VAL); }

  static vector<MyBoundedType> shrink(const MyBoundedType&)
  { return vector<MyBoundedType>(); }
};

DECLARE_PROPERTY(Bounded, Property, const MyBoundedType& m)
{
  return m.m_val < MyBoundedType::MAX_VAL;
}
