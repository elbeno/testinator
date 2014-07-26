#include <property.h>

using namespace std;

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
struct FuncTraitsStruct
{
  void operator()(int);
};

DECLARE_TEST(FunctorTraits, Property)
{
  typedef testpp::function_traits<FuncTraitsStruct> traits;

  return std::is_same<int, traits::argType>::value;
}

//------------------------------------------------------------------------------
DECLARE_PROPERTY(StringReverse, Property, const string& s)
{
  string r(s);
  reverse(r.begin(), r.end());
  reverse(r.begin(), r.end());
  return s == r;
}

//------------------------------------------------------------------------------
DECLARE_PROPERTY(StringLacksA, Property, const string& s)
{
  return s.find('A') == s.npos;
}

//------------------------------------------------------------------------------
struct MyType
{
  MyType() : m_val(1337) {}
  int m_val;
};

template <>
struct testpp::Arbitrary<MyType, false>
{
  static MyType generate(std::size_t generation) { return MyType(); }
  static std::vector<MyType> shrink(const MyType& t) { return std::vector<MyType>(); }
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
struct MyUnspecializedType
{
  MyUnspecializedType() : m_val(1337) {}
  int m_val;
};

ostream& operator<<(ostream& s, const MyUnspecializedType& m)
{
  return s << m.m_val;
}

DECLARE_PROPERTY(MyUnspecializedType, Property, const MyUnspecializedType& m)
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
