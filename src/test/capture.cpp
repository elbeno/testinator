// Copyright (c) 2014-2016 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#include <testinator.h>

#include <memory>
#include <ostream>
#include <sstream>
#include <string>

using namespace std;

//------------------------------------------------------------------------------
struct ETCaptureTestValue
{
  ~ETCaptureTestValue() { x = 2; }
  int x = 1;
};

auto make_capture_value() { return ETCaptureTestValue{}; }

bool operator==(int x, const ETCaptureTestValue& y) { return x == y.x; }
bool operator==(const ETCaptureTestValue& y, int x) { return y.x == x; }
ostream& operator<<(ostream&s , const ETCaptureTestValue& x) { return s << x.x; }

//------------------------------------------------------------------------------
class TestETCaptureLValueLeft : public testinator::Test
{
public:
  TestETCaptureLValueLeft(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    ETCaptureTestValue v;
    EXPECT(v == 0);
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(ETCaptureLValueLeft, Capture)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestETCaptureLValueLeft myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "1 == 0";
  return oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class TestETCaptureLValueRight : public testinator::Test
{
public:
  TestETCaptureLValueRight(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    ETCaptureTestValue v;
    EXPECT(0 == v);
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(ETCaptureLValueRight, Capture)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestETCaptureLValueRight myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "0 == 1";
  return oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class TestETCaptureRValueLeft : public testinator::Test
{
public:
  TestETCaptureRValueLeft(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    EXPECT(make_capture_value() == 0);
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(ETCaptureRValueLeft, Capture)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestETCaptureRValueLeft myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "1 == 0";
  return oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class TestETCaptureRValueRight : public testinator::Test
{
public:
  TestETCaptureRValueRight(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    EXPECT(0 == make_capture_value());
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(ETCaptureRValueRight, Capture)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestETCaptureRValueRight myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "0 == 1";
  return oss.str().find(expected) != string::npos;
}
