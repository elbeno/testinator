#include <testinator.h>

using namespace std;

//------------------------------------------------------------------------------
namespace
{
  static const char* s_suiteName = "Test";
}

//------------------------------------------------------------------------------
class TestCallTest : public testinator::Test
{
public:
  TestCallTest(const string& name) : testinator::Test(name, s_suiteName) {}

  virtual bool Run()
  {
    testinator::Test myTest("call_test");
    testinator::Results rs = testinator::RunTest("call_test");
    return !rs.empty() && rs.front().m_success;
  }
};

//------------------------------------------------------------------------------
class TestSetupFirst : public testinator::Test
{
public:
  TestSetupFirst(const string& name)
    : testinator::Test(name, s_suiteName)
    , m_setupCalled(false)
  {}

  virtual bool Setup(const testinator::RunParams&)
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
class TestTeardownAfterwardsInternal : public testinator::Test
{
public:
  TestTeardownAfterwardsInternal(const string& name, bool forceFail = false)
    : testinator::Test(name)
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
class TestTeardownAfterwards : public testinator::Test
{
public:
  TestTeardownAfterwards(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestTeardownAfterwardsInternal myTest("teardown_test");
    testinator::RunTest("teardown_test");
    return myTest.m_teardownCalled;
  }
};

//------------------------------------------------------------------------------
class TestTeardownAfterFail : public testinator::Test
{
public:
  TestTeardownAfterFail(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestTeardownAfterwardsInternal myTest("teardown_test", true);
    testinator::RunTest("teardown_test");
    return myTest.m_teardownCalled;
  }
};

//------------------------------------------------------------------------------
class TestRunMultipleInternal : public testinator::Test
{
public:
  TestRunMultipleInternal(const string& name)
    : testinator::Test(name)
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
class TestRunMultiple : public testinator::Test
{
public:
  TestRunMultiple(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestRunMultipleInternal test0("test0");
    TestRunMultipleInternal test1("test1");
    testinator::Results rs = testinator::RunAllTests();
    return test0.m_runCalled && test1.m_runCalled;
  }
};

//------------------------------------------------------------------------------
class TestReportResultsInternal : public testinator::Test
{
public:
  TestReportResultsInternal(const string& name, bool fail)
    : testinator::Test(name)
    , m_fail(fail)
  {}

  virtual bool Run()
  {
    return !m_fail;
  }

  bool m_fail;
};

//------------------------------------------------------------------------------
class TestReportResults : public testinator::Test
{
public:
  TestReportResults(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    TestReportResultsInternal test0("expected_fail", true);
    TestReportResultsInternal test1("expected_pass", false);
    testinator::Results rs = testinator::RunAllTests();
    auto numPassed = count_if(rs.begin(), rs.end(),
                              [] (const testinator::Result& r) { return r.m_success; });
    auto total = decltype(numPassed)(rs.size());
    auto numFailed = total - numPassed;
    return numPassed == 1 && numFailed == 1;
  }
};

//------------------------------------------------------------------------------
class TestRunSuite : public testinator::Test
{
public:
  TestRunSuite(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::Test myTest1("test1", "suite1");
    testinator::Test myTest2("test2", "suite2");
    testinator::Results rs = testinator::RunSuite("suite1");
    auto numPassed = count_if(rs.begin(), rs.end(),
                              [] (const testinator::Result& r) { return r.m_success; });
    return numPassed == 1;
  }
};

//------------------------------------------------------------------------------
class TestCheckMacroInternal : public testinator::Test
{
public:
  TestCheckMacroInternal(const string& name)
    : testinator::Test(name)
  {}

  virtual bool Run()
  {
    bool fail = true;
    EXPECT(!fail == fail);
    return true;
  }
};

//------------------------------------------------------------------------------
class TestCheckMacro : public testinator::Test
{
public:
  TestCheckMacro(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    std::unique_ptr<testinator::DefaultOutputter> op =
      make_unique<testinator::DefaultOutputter>(oss);
    TestCheckMacroInternal myTestA("A");
    testinator::Results rs = testinator::RunAllTests(testinator::RunParams(), op.get());

    static string expected =
      "EXPECT FAILED: build/debug/test/main.cpp:211 (!fail == fail => false == true)";

    return !rs.empty() && !rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestDiagnosticInternal : public testinator::Test
{
public:
  TestDiagnosticInternal(const string& name)
    : testinator::Test(name)
  {}

  virtual bool Run()
  {
    DIAGNOSTIC("Hello world " << 42);
    return true;
  }
};

//------------------------------------------------------------------------------
class TestDiagnostic : public testinator::Test
{
public:
  TestDiagnostic(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    std::unique_ptr<testinator::DefaultOutputter> op =
      make_unique<testinator::DefaultOutputter>(oss);
    TestDiagnosticInternal myTestA("A");
    testinator::Results rs = testinator::RunAllTests(testinator::RunParams(), op.get());

    static string expected = "Hello world 42";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(Macro, Test)
{
  return true;
}

//------------------------------------------------------------------------------
namespace
{
  size_t s_numPropertyChecks = 0;
}

DEF_PROPERTY(NumChecks, Property, int)
{
  return m_numChecks == s_numPropertyChecks;
}

//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  string outputterName;
  string testName;
  string suiteName;
  testinator::RunParams p;
  auto oflags = testinator::OF_COLOR|testinator::OF_QUIET_SUCCESS;

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
        p.m_flags |= testinator::RF_ALPHA_ORDER;
        continue;
      }
    }

    {
      string option = "--verbose";
      if (s.compare(0, option.size(), option) == 0)
      {
        oflags &= ~(static_cast<underlying_type_t<testinator::OutputFlags>>(
                        testinator::OF_QUIET_SUCCESS));
        continue;
      }
    }

    {
      string option = "--nocolor";
      if (s.compare(0, option.size(), option) == 0)
      {
        oflags &= ~(static_cast<underlying_type_t<testinator::OutputFlags>>(
                        testinator::OF_COLOR));
        continue;
      }
    }

    {
      std::string option = "--help";
      if (s.compare(0, option.size(), option) == 0)
      {
        std::cout << "Usage: testinator [OPTION]..." << std::endl
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
  TestDiagnostic test8("TestDiagnostic");
  testinator::Results rs;

  std::unique_ptr<testinator::Outputter> op = testinator::MakeOutputter(
      outputterName, static_cast<testinator::OutputFlags>(oflags));

  if (!testName.empty())
    rs = testinator::RunTest(testName, p, op.get());
  else if (!suiteName.empty())
    rs = testinator::RunSuite(suiteName, p, op.get());
  else
    rs = testinator::RunAllTests(p, op.get());

  auto numPassed = count_if(rs.begin(), rs.end(),
                            [] (const testinator::Result& r) { return r.m_success; });
  auto total = static_cast<decltype(numPassed)>(rs.size());
  return total - numPassed;
}
