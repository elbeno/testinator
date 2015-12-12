// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#include <property.h>

using namespace std;

//------------------------------------------------------------------------------
// A test of the machinery

struct TestFunctor
{
  bool operator()(int) const { return true; }
  unsigned long m_randomSeed = 0;
};

DEF_TEST(Functor, Property)
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

DEF_TEST(FunctorTraits, Property)
{
  using traits = testinator::function_traits<FuncTraitsStruct>;

  return std::is_same<std::tuple<int>, traits::argTuple>::value;
}

//------------------------------------------------------------------------------
// A simple test of string reversal invariant

DEF_PROPERTY(StringReverse, Property, const string& s)
{
  string r(s);
  reverse(r.begin(), r.end());
  reverse(r.begin(), r.end());
  return s == r;
}

//------------------------------------------------------------------------------
class FailOnEmptyStringInternal : public testinator::PropertyTest
{
public:
  FailOnEmptyStringInternal(testinator::TestRegistry& r,
                            const string& name, const string& suite)
    : testinator::PropertyTest(r, name, suite)
  {}

  virtual bool Run() override
  {
    testinator::Property p(*this);
    return p.check(m_numChecks, m_op);
  }

  bool operator()(const string& s)
  {
    return !s.empty();
  }
};

DEF_TEST(FailOnEmptyString, Property)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);
  FailOnEmptyStringInternal myTestA(r, "A", "Property");

  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "Failed (\"\")";
  return !rs.empty() && !rs.front().m_success
    && oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class FailTriggersShrinkA1Internal : public testinator::PropertyTest
{
public:
  FailTriggersShrinkA1Internal(testinator::TestRegistry& r,
                               const string& name, const string& suite)
    : testinator::PropertyTest(r, name, suite)
  {}

  virtual bool Run() override
  {
    testinator::Property p(*this);
    return p.check(m_numChecks, m_op);
  }

  bool operator()(const string& s)
  {
    return s.find('A') == string::npos;
  }
};

DEF_TEST(FailTriggersShrink1, Property)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);
  FailTriggersShrinkA1Internal myTestA(r, "A", "Property");

  testinator::RunParams p;
  p.m_randomSeed = 886871573;
  testinator::Results rs = r.RunAllTests(p, op.get());

  static string expected = "Failed (\"A\")";
  return !rs.empty() && !rs.front().m_success
    && oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class FailTriggersShrinkA2Internal : public testinator::PropertyTest
{
public:
  FailTriggersShrinkA2Internal(testinator::TestRegistry& r,
                               const string& name, const string& suite)
    : testinator::PropertyTest(r, name, suite)
  {}

  virtual bool Run() override
  {
    testinator::Property p(*this);
    return p.check(m_numChecks, m_op);
  }

  bool operator()(const string& s)
  {
    return !s.empty() && s.find('A') == string::npos;
  }
};

DEF_TEST(FailTriggersShrink2, Property)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);
  FailTriggersShrinkA2Internal myTestA(r, "A", "Property");

  testinator::RunParams p;
  p.m_randomSeed = 886871573;
  testinator::Results rs = r.RunAllTests(p, op.get());

  static string expected = "Failed (\"\")";
  return !rs.empty() && !rs.front().m_success
    && oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class FailTriggersShrinkBInternal : public testinator::PropertyTest
{
public:
  FailTriggersShrinkBInternal(testinator::TestRegistry& r,
                              const string& name, const string& suite)
    : testinator::PropertyTest(r, name, suite)
  {}

  virtual bool Run() override
  {
    testinator::Property p(*this);
    return p.check(m_numChecks, m_op);
  }

  bool operator()(const string& s1, const string& s2)
  {
    return s1.find('A') == string::npos
      || s2.find('B') == string::npos;
  }
};

DEF_TEST(FailTriggersShrinkMulti, Property)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);
  FailTriggersShrinkBInternal myTestA(r, "A", "Property");

  testinator::RunParams p;
  p.m_randomSeed = 3358856245;
  testinator::Results rs = r.RunAllTests(p, op.get());

  static string expected = "Failed (\"A\",\"B\")";
  return !rs.empty() && !rs.front().m_success
    && oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class FailTriggersShrinkCInternal : public testinator::PropertyTest
{
public:
  FailTriggersShrinkCInternal(testinator::TestRegistry& r,
                              const string& name, const string& suite)
    : testinator::PropertyTest(r, name, suite)
  {}

  virtual bool Run() override
  {
    testinator::Property p(*this);
    return p.check(m_numChecks, m_op);
  }

  bool operator()(const std::pair<string, string>& p)
  {
    return p.first.find('A') == string::npos
      || p.second.find('B') == string::npos;
  }
};

DEF_TEST(FailTriggersShrinkPair, Property)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);
  FailTriggersShrinkCInternal myTestA(r, "A", "Property");

  testinator::RunParams p;
  p.m_randomSeed = 932579536;
  testinator::Results rs = r.RunAllTests(p, op.get());

  static string expected = "Failed ((\"A\",\"B\"))";
  return !rs.empty() && !rs.front().m_success
    && oss.str().find(expected) != string::npos;
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

DEF_PROPERTY(MyType, Property, const MyType& m)
{
  return m.m_val == 1337;
}

//------------------------------------------------------------------------------
DEF_PROPERTY(ConstInt, Property, const int)
{
  return true;
}

//------------------------------------------------------------------------------
DEF_PROPERTY(ConstChar, Property, const char)
{
  return true;
}

//------------------------------------------------------------------------------
DEF_PROPERTY(pair, Property, const std::pair<int, float>&)
{
  return true;
}

//------------------------------------------------------------------------------
DEF_PROPERTY(tuple, Property, const std::tuple<int, float>&)
{
  return true;
}

//------------------------------------------------------------------------------
DEF_PROPERTY(twoargs, Property, int, float)
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

namespace testinator
{
  template <>
  struct Arbitrary<MyBoundedType>
  {
    static MyBoundedType generate(std::size_t generation, unsigned long int /*randomSeed*/)
    { return MyBoundedType(generation % MyBoundedType::MAX_VAL); }

    static vector<MyBoundedType> shrink(const MyBoundedType&)
    { return vector<MyBoundedType>(); }
  };
}

DEF_PROPERTY(Bounded, Property, const MyBoundedType& m)
{
  return m.m_val < MyBoundedType::MAX_VAL;
}

//------------------------------------------------------------------------------
// Test a container with a proxy iterator

DEF_PROPERTY(ProxyIterator, Property, const vector<bool>&)
{
  return true;
}
