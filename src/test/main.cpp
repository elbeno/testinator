#include <test_extended.h>

#include <iostream>
#include <map>
#include <string>
#include <sstream>

using namespace std;

//------------------------------------------------------------------------------
namespace
{
  static const char* s_suiteName = "Test";
}

//------------------------------------------------------------------------------
class TestCallTest : public testpp::Test
{
public:
  TestCallTest(const char* name) : testpp::Test(name, s_suiteName) {}

  virtual bool Run()
  {
    testpp::Test myTest("call_test");
    return testpp::Run("call_test");
  }
};

//------------------------------------------------------------------------------
class TestSetupFirst : public testpp::Test
{
public:
  TestSetupFirst(const char* name)
    : testpp::Test(name, s_suiteName)
    , m_setupCalled(false)
  {}

  virtual bool Setup()
  {
    m_setupCalled = true;
    return true;
  }

  virtual bool Run()
  {
    return m_setupCalled;
  }

private:
  bool m_setupCalled;
};

//------------------------------------------------------------------------------
class TestTeardownAfterwardsInternal : public testpp::Test
{
public:
  TestTeardownAfterwardsInternal(const char* name, bool forceFail = false)
    : testpp::Test(name)
    , m_runCalled(false)
    , m_teardownCalled(false)
    , m_forceFail(forceFail)
  {}

  virtual void Teardown()
  {
    m_teardownCalled = m_runCalled;
  }

  virtual bool Run()
  {
    m_runCalled = true;
    return !m_forceFail;
  }

  bool m_runCalled;
  bool m_teardownCalled;
  bool m_forceFail;
};

//------------------------------------------------------------------------------
class TestTeardownAfterwards : public testpp::Test
{
public:
  TestTeardownAfterwards(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestTeardownAfterwardsInternal myTest("teardown_test");
    testpp::Run("teardown_test");
    return myTest.m_teardownCalled;
  }
};

//------------------------------------------------------------------------------
class TestTeardownAfterFail : public testpp::Test
{
public:
  TestTeardownAfterFail(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestTeardownAfterwardsInternal myTest("teardown_test", true);
    testpp::Run("teardown_test");
    return myTest.m_teardownCalled;
  }
};

//------------------------------------------------------------------------------
class TestRunMultipleInternal : public testpp::Test
{
public:
  TestRunMultipleInternal(const char* name)
    : testpp::Test(name)
    , m_runCalled(false)
  {}

  virtual bool Run()
  {
    m_runCalled = true;
    return true;
  }

  bool m_runCalled;
};

//------------------------------------------------------------------------------
class TestRunMultiple : public testpp::Test
{
public:
  TestRunMultiple(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestRunMultipleInternal test0("test0");
    TestRunMultipleInternal test1("test1");

    testpp::Results r;
    testpp::RunAllTests(r, testpp::QUIET);

    return test0.m_runCalled && test1.m_runCalled;
  }
};

//------------------------------------------------------------------------------
class TestReportResultsInternal : public testpp::Test
{
public:
  TestReportResultsInternal(const char* name, bool fail)
    : testpp::Test(name)
    , m_fail(fail)
  {}

  virtual bool Run()
  {
    return !m_fail;
  }

  bool m_fail;
};

//------------------------------------------------------------------------------
class TestReportResults : public testpp::Test
{
public:
  TestReportResults(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestReportResultsInternal test0("test0", true);
    TestReportResultsInternal test1("test1", false);

    testpp::Results r;
    testpp::RunAllTests(r, testpp::QUIET);

    return r.m_numPassed == 1 && r.m_numFailed == 1;
  }
};

//------------------------------------------------------------------------------
class TestOutputToStream : public testpp::Test
{
public:
  TestOutputToStream(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::Test myTest("call_test");
    testpp::Results r;
    testpp::RunAllTests(r, testpp::NONE, oss);

    static string expected = "PASS: ::call_test\n";
    return oss.str() == expected;
  }
};

//------------------------------------------------------------------------------
class TestColorOutput : public testpp::Test
{
public:
  TestColorOutput(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::Test myTest("call_test");
    testpp::Results r;
    testpp::RunAllTests(r, testpp::COLOR, oss);

    static string expected = "\033[32;1mPASS\033[0m: ::call_test\n";
    return oss.str() == expected;
  }
};

//------------------------------------------------------------------------------
class TestRunSuite : public testpp::Test
{
public:
  TestRunSuite(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testpp::Test myTest1("test1", "suite1");
    testpp::Test myTest2("test2", "suite2");

    testpp::Results r;
    testpp::RunSuite("suite1", r, testpp::QUIET);

    return r.m_numPassed == 1;
  }
};

//------------------------------------------------------------------------------
class TestQuietSuccess : public testpp::Test
{
public:
  TestQuietSuccess(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::Test myTest("call_test");
    testpp::Results r;
    testpp::RunAllTests(r, testpp::QUIET_SUCCESS, oss);

    static string expected = "";
    return oss.str() == expected;
  }
};

//------------------------------------------------------------------------------
class TestAlphaOrder : public testpp::Test
{
public:
  TestAlphaOrder(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::Test myTestA("A");
    testpp::Test myTestB("B");
    testpp::Test myTestC("C");
    testpp::Test myTestD("D");
    testpp::Test myTestE("E");
    testpp::Results r;
    testpp::RunAllTests(r, testpp::ALPHA_ORDER, oss);

    static string expected = "PASS: ::A\nPASS: ::B\nPASS: ::C\n"
      "PASS: ::D\nPASS: ::E\n";
    return oss.str() == expected;
  }
};

//------------------------------------------------------------------------------
class TestRandomOrder : public testpp::Test
{
public:
  TestRandomOrder(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testpp::Test myTestA("A");
    testpp::Test myTestB("B");
    testpp::Test myTestC("C");
    testpp::Test myTestD("D");
    testpp::Test myTestE("E");
    testpp::Results r;

    ostringstream oss1;
    testpp::RunAllTests(r, testpp::NONE, oss1);
    ostringstream oss2;
    testpp::RunAllTests(r, testpp::NONE, oss2);

    return oss1.str() != oss2.str();
  }
};

//------------------------------------------------------------------------------
class TestCheckMacroInternal : public testpp::Test
{
public:
  TestCheckMacroInternal(const char* name, bool fail)
    : testpp::Test(name)
    , m_fail(fail)
  {}

  virtual bool Run()
  {
    EXPECT_NOT(m_fail);
    EXPECT(!m_fail);
    return true;
  }
private:
  bool m_fail;
};

//------------------------------------------------------------------------------
class TestCheckMacro : public testpp::Test
{
public:
  TestCheckMacro(const char* name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    TestCheckMacroInternal myTestA("A", true);
    TestCheckMacroInternal myTestB("B", false);
    testpp::Results r;
    testpp::RunAllTests(r, testpp::ALPHA_ORDER, oss);

    static string expected =
      "EXPECT_NOT FAILED: build/debug/test/main.cpp:329 (m_fail)\n"
      "EXPECT FAILED: build/debug/test/main.cpp:330 (!m_fail)\n"
      "FAIL: ::A\n"
      "PASS: ::B\n";
    return oss.str() == expected;
  }
};

//------------------------------------------------------------------------------
DECLARE_TEST(Macro, Test)
{
  return true;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  string suiteName;

  for (int i = 1; i < argc; ++i)
  {
    string s = argv[i];

    {
      string option = "--suiteName=";
      if (s.compare(0, option.size(), option) == 0)
      {
        suiteName = s.substr(option.size());
        continue;
      }
    }
  }

  TestCallTest test0("TestCallTest");
  TestSetupFirst test1("TestSetupFirst");
  TestTeardownAfterwards test2("TestTeardownAfterwards");
  TestTeardownAfterFail test3("TestTeardownAfterFail");
  TestRunMultiple test4("TestRunMultiple");
  TestReportResults test5("TestReportResults");
  TestOutputToStream test6("TestOutputToStream");
  TestColorOutput test7("TestColorOutput");
  TestRunSuite test8("TestRunSuite");
  TestQuietSuccess test9("TestQuietSuccess");
  TestAlphaOrder test10("TestAlphaOrder");
  TestRandomOrder test11("TestRandomOrder");
  TestCheckMacro test12("TestCheckMacro");

  testpp::Results r;

  if (suiteName.empty())
    testpp::RunAllTests(r);
  else
    testpp::RunSuite(suiteName.c_str(), r);

  cout << r.m_numPassed << "/" << r.m_numPassed + r.m_numFailed
       << " tests passed." << endl;

  return r.m_numFailed;
}
