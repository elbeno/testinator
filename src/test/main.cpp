#include <testpp.h>

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
    testpp::Test myTest("call_test");
    testpp::Results rs = testpp::RunTest("call_test");
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
    TestTeardownAfterwardsInternal myTest("teardown_test");
    testpp::RunTest("teardown_test");
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
    TestTeardownAfterwardsInternal myTest("teardown_test", true);
    testpp::RunTest("teardown_test");
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
    testpp::Results rs = testpp::RunAllTests();
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
    testpp::Results rs = testpp::RunAllTests();
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
    testpp::Results rs = testpp::RunSuite("suite1");
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
    std::unique_ptr<testpp::DefaultOutputter> op =
      make_unique<testpp::DefaultOutputter>(oss);
    TestCheckMacroInternal myTestA("A", true);
    testpp::Results rs = testpp::RunAllTests(testpp::RunParams(), op.get());

    static string expected =
      "EXPECT_NOT FAILED: build/debug/test/main.cpp:211 (m_fail)";

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
  string outputterName;
  string testName;
  string suiteName;
  testpp::RunParams p;
  auto oflags = testpp::OF_COLOR|testpp::OF_QUIET_SUCCESS;

  for (int i = 1; i < argc; ++i)
  {
    string s = argv[i];

    {
      string option = "--output=";
      if (s.compare(0, option.size(), option) == 0)
      {
        outputterName = s.substr(option.size());
        continue;
      }
    }

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
        p.m_flags |= testpp::RF_ALPHA_ORDER;
        continue;
      }
    }

    {
      string option = "--verbose";
      if (s.compare(0, option.size(), option) == 0)
      {
        oflags &= ~(static_cast<underlying_type_t<testpp::OutputFlags>>(
                        testpp::OF_QUIET_SUCCESS));
        continue;
      }
    }

    {
      string option = "--nocolor";
      if (s.compare(0, option.size(), option) == 0)
      {
        oflags &= ~(static_cast<underlying_type_t<testpp::OutputFlags>>(
                        testpp::OF_COLOR));
        continue;
      }
    }

    {
      std::string option = "--help";
      if (s.compare(0, option.size(), option) == 0)
      {
        std::cout << "Usage: testpp [OPTION]..." << std::endl
                  << "Run all tests in randomized order by default." << std::endl
                  << std::endl
                  << "--testName=NAME    run only the named test" << std::endl
                  << "--suiteName=NAME   run only the tests in the named suite" << std::endl
                  << "--alpha            run tests in alphabetical order" << std::endl
                  << "--output=FORMAT    use the specified output formatter, e.g. TAP" << std::endl
                  << "--verbose          give verbose output (according to formatter)" << std::endl
                  << "--nocolor          output without ANSI color codes (according to formatter)"
                  << std::endl
                  << "--numChecks=N      number of checks to use for property tests" << std::endl
                  << "--seed=SEED        use SEED for property test randomization" << std::endl;
        return 0;
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

  std::unique_ptr<testpp::Outputter> op = testpp::MakeOutputter(
      outputterName, static_cast<testpp::OutputFlags>(oflags));

  if (!testName.empty())
    rs = testpp::RunTest(testName, p, op.get());
  else if (!suiteName.empty())
    rs = testpp::RunSuite(suiteName, p, op.get());
  else
    rs = testpp::RunAllTests(p, op.get());

  auto numPassed = count_if(rs.begin(), rs.end(),
                            [] (const testpp::Result& r) { return r.m_success; });
  auto total = static_cast<decltype(numPassed)>(rs.size());
  return total - numPassed;
}
