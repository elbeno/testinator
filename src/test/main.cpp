#include <output.h>
#include <property.h>
#include <test.h>

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
  TestCallTest(const string& name) : testpp::Test(name, s_suiteName) {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    testpp::Test myTest("call_test");
    testpp::Results rs = testpp::RunTest("call_test", p);
    return !rs.empty() && rs.front().m_success;
  }
};

//------------------------------------------------------------------------------
class TestSetupFirst : public testpp::Test
{
public:
  TestSetupFirst(const string& name)
    : testpp::Test(name, s_suiteName)
    , m_setupCalled(false)
  {}

  virtual bool Setup(const testpp::RunParams&)
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
  TestTeardownAfterwardsInternal(const string& name, bool forceFail = false)
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
  TestTeardownAfterwards(const string& name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    TestTeardownAfterwardsInternal myTest("teardown_test");
    testpp::RunTest("teardown_test", p);
    return myTest.m_teardownCalled;
  }
};

//------------------------------------------------------------------------------
class TestTeardownAfterFail : public testpp::Test
{
public:
  TestTeardownAfterFail(const string& name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    TestTeardownAfterwardsInternal myTest("teardown_test", true);
    testpp::RunTest("teardown_test", p);
    return myTest.m_teardownCalled;
  }
};

//------------------------------------------------------------------------------
class TestRunMultipleInternal : public testpp::Test
{
public:
  TestRunMultipleInternal(const string& name)
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
  TestRunMultiple(const string& name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestRunMultipleInternal test0("test0");
    TestRunMultipleInternal test1("test1");
    ostringstream oss;
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    testpp::Results rs = testpp::RunAllTests(p);
    return test0.m_runCalled && test1.m_runCalled;
  }
};

//------------------------------------------------------------------------------
class TestReportResultsInternal : public testpp::Test
{
public:
  TestReportResultsInternal(const string& name, bool fail)
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
  TestReportResults(const string& name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestReportResultsInternal test0("expected_fail", true);
    TestReportResultsInternal test1("expected_pass", false);
    ostringstream oss;
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    testpp::Results rs = testpp::RunAllTests(p);

    auto numPassed = count_if(rs.begin(), rs.end(),
                              [] (const testpp::Result& r) { return r.m_success; });
    auto total = decltype(numPassed)(rs.size());
    auto numFailed = total - numPassed;
    return numPassed == 1 && numFailed == 1;
  }
};

//------------------------------------------------------------------------------
class TestRunSuite : public testpp::Test
{
public:
  TestRunSuite(const string& name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testpp::Test myTest1("test1", "suite1");
    testpp::Test myTest2("test2", "suite2");
    ostringstream oss;
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    testpp::Results rs = testpp::RunSuite("suite1", p);

    auto numPassed = count_if(rs.begin(), rs.end(),
                              [] (const testpp::Result& r) { return r.m_success; });
    return numPassed == 1;
  }
};

//------------------------------------------------------------------------------
class TestCheckMacroInternal : public testpp::Test
{
public:
  TestCheckMacroInternal(const string& name, bool fail)
    : testpp::Test(name)
    , m_fail(fail)
  {}

  virtual bool Run()
  {
    EXPECT_NOT(m_fail);
    return true;
  }
private:
  bool m_fail;
};

//------------------------------------------------------------------------------
class TestCheckMacro : public testpp::Test
{
public:
  TestCheckMacro(const string& name)
    : testpp::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    TestCheckMacroInternal myTestA("A", true);
    testpp::RunParams p{testpp::DefaultOutputter(oss)};
    testpp::Results rs = testpp::RunAllTests(p);

    static string expected =
      "EXPECT_NOT FAILED: build/debug/test/main.cpp:227 (m_fail)";

    return !rs.empty() && !rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
DECLARE_TEST(Macro, Test)
{
  return true;
}

//------------------------------------------------------------------------------
namespace
{
  size_t s_numPropertyChecks = 0;
}

DECLARE_PROPERTY(NumChecks, Property, int)
{
  return m_numChecks == s_numPropertyChecks;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  string testName;
  string suiteName;
  testpp::RunParams p{testpp::DefaultOutputter()};

  for (int i = 1; i < argc; ++i)
  {
    string s = argv[i];

    {
      string option = "--testName=";
      if (s.compare(0, option.size(), option) == 0)
      {
        testName = s.substr(option.size());
        continue;
      }
    }

    {
      string option = "--suiteName=";
      if (s.compare(0, option.size(), option) == 0)
      {
        suiteName = s.substr(option.size());
        continue;
      }
    }

    {
      string option = "--numChecks=";
      if (s.compare(0, option.size(), option) == 0)
      {
        char* end;
        p.m_numPropertyChecks = strtoul(s.substr(option.size()).c_str(), &end, 10);
        continue;
      }
    }

    {
      string option = "--seed=";
      if (s.compare(0, option.size(), option) == 0)
      {
        char* end;
        p.m_randomSeed = strtoul(s.substr(option.size()).c_str(), &end, 10);
        continue;
      }
    }

    {
      string option = "--alpha";
      if (s.compare(0, option.size(), option) == 0)
      {
        p.m_flags |= testpp::ALPHA_ORDER;
        continue;
      }
    }

    {
      string option = "--verbose";
      if (s.compare(0, option.size(), option) == 0)
      {
        p.m_flags &= ~(static_cast<unsigned int>(testpp::QUIET_SUCCESS));
        continue;
      }
    }
  }

  s_numPropertyChecks = p.m_numPropertyChecks;
  TestCallTest test0("TestCallTest");
  TestSetupFirst test1("TestSetupFirst");
  TestTeardownAfterwards test2("TestTeardownAfterwards");
  TestTeardownAfterFail test3("TestTeardownAfterFail");
  TestRunMultiple test4("TestRunMultiple");
  TestReportResults test5("TestReportResults");
  TestRunSuite test6("TestRunSuite");
  TestCheckMacro test7("TestCheckMacro");
  testpp::Results rs;

  if (!testName.empty())
    rs = testpp::RunTest(testName, p);
  else if (!suiteName.empty())
    rs = testpp::RunSuite(suiteName, p);
  else
    rs = testpp::RunAllTests(p);

  auto numPassed = count_if(rs.begin(), rs.end(),
                            [] (const testpp::Result& r) { return r.m_success; });
  auto total = static_cast<decltype(numPassed)>(rs.size());
  return total - numPassed;
}
