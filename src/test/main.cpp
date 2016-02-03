// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#include <testinator.h>

#include <iostream>
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
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    testinator::Test myTest(r, "call_test");
    testinator::Results rs = r.RunTest("call_test", testinator::RunParams(), op.get());
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
class TestRunMultipleInternal : public testinator::Test
{
public:
  TestRunMultipleInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
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
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestRunMultipleInternal test0(r, "test0");
    TestRunMultipleInternal test1(r, "test1");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());
    return test0.m_runCalled && test1.m_runCalled;
  }
};

//------------------------------------------------------------------------------
class TestReportResultsInternal : public testinator::Test
{
public:
  TestReportResultsInternal(testinator::TestRegistry& r,
                            const string& name, bool fail)
    : testinator::Test(r, name)
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
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestReportResultsInternal test0(r, "expected_fail", true);
    TestReportResultsInternal test1(r, "expected_pass", false);
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());
    auto numPassed = count_if(rs.begin(), rs.end(),
                              [] (const testinator::Result& res)
                              { return res.m_success; });
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
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    testinator::Test myTest1(r, "test1", "suite1");
    testinator::Test myTest2(r, "test2", "suite2");
    testinator::Results rs = r.RunSuite("suite1", testinator::RunParams(), op.get());
    auto numPassed = count_if(rs.begin(), rs.end(),
                              [] (const testinator::Result& res)
                              { return res.m_success; });
    return numPassed == 1;
  }
};

//------------------------------------------------------------------------------
class TestExpectMacroInternal : public testinator::Test
{
public:
  TestExpectMacroInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    bool fail = true;
    EXPECT(!fail == fail);
    return true;
  }
};

//------------------------------------------------------------------------------
class TestExpectMacro : public testinator::Test
{
public:
  TestExpectMacro(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestExpectMacroInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected =
      "main.cpp:179 (!fail == fail => false == true)";
    return !rs.empty() && !rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
struct CountEvals
{
  bool inc() { ++s_evals; return true; }
  static int s_evals;
};
int CountEvals::s_evals = 0;

class TestExpectEvalsOnceInternal : public testinator::Test
{
public:
  TestExpectEvalsOnceInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    CountEvals c;
    EXPECT(c.inc() == c.inc());
    EXPECT(CountEvals::s_evals == 2);
    return true;
  }
};

//------------------------------------------------------------------------------
class TestExpectEvalsOnce : public testinator::Test
{
public:
  TestExpectEvalsOnce(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestExpectEvalsOnceInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string notexpected = "EXPECT FAILED";

    return !rs.empty() && rs.front().m_success
      && oss.str().find(notexpected) == string::npos;
  }
};

//------------------------------------------------------------------------------
class TestDiagnosticInternal : public testinator::Test
{
public:
  TestDiagnosticInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
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
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestDiagnosticInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected = "Hello world 42";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestAbortInternal : public testinator::Test
{
public:
  TestAbortInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    m_runCalled = true;
    ABORT("Hello world " << 42);
    return true;
  }

  bool m_runCalled = false;
};

//------------------------------------------------------------------------------
class TestAbort : public testinator::Test
{
public:
  TestAbort(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss, testinator::OF_NONE);

    TestAbortInternal myTestA(r, "A");
    TestAbortInternal myTestB(r, "B");
    testinator::RunParams params;
    params.m_flags = testinator::RF_ALPHA_ORDER;
    testinator::Results rs = r.RunAllTests(params, op.get());

    static string expected = "ABORT (Hello world 42)";
    return !myTestB.m_runCalled
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestSkipInternal : public testinator::Test
{
public:
  TestSkipInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    SKIP("Hello world " << 42);
    return false;
  }
};

//------------------------------------------------------------------------------
class TestSkip : public testinator::Test
{
public:
  TestSkip(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestSkipInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    return myTestA.skipped();
  }
};

//------------------------------------------------------------------------------
class TestBranchInternal : public testinator::Test
{
public:
  TestBranchInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Run()
  {
    DIAGNOSTIC("no branch");

    BRANCH(A)
    {
      DIAGNOSTIC("branch " << BRANCH_NAME);
    }

    BRANCH(B)
    {
      DIAGNOSTIC("branch " << BRANCH_NAME);
    }

    return true;
  }
};

//------------------------------------------------------------------------------
class TestBranch : public testinator::Test
{
public:
  TestBranch(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestBranchInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected =
      "no branch\nbranch A\nno branch\nbranch B";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestBranchInternal2 : public testinator::Test
{
public:
  TestBranchInternal2(const string& name)
    : testinator::Test(name)
  {}

  virtual bool Run()
  {
    BRANCH()
    {
      DIAGNOSTIC("branch " << BRANCH_NAME);
    }

    return true;
  }
};

//------------------------------------------------------------------------------
class TestBranchNoName : public testinator::Test
{
public:
  TestBranchNoName(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);
    TestBranchInternal2 myTestA("A");
    testinator::Results rs = testinator::RunAllTests(testinator::RunParams(), op.get());

    static string expected = "main.cpp:440";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestTAPSkip : public testinator::Test
{
public:
  TestTAPSkip(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::TAPOutputter>(oss);

    TestSkipInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected = "# skip Hello world 42";
    return myTestA.skipped()
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestTAPDiagnostic : public testinator::Test
{
public:
  TestTAPDiagnostic(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::TAPOutputter>(oss);

    TestDiagnosticInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected = "# Hello world 42";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestTAPAbort : public testinator::Test
{
public:
  TestTAPAbort(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::TAPOutputter>(oss);

    TestAbortInternal myTestA(r, "A");
    TestAbortInternal myTestB(r, "B");
    testinator::RunParams params;
    params.m_flags = testinator::RF_ALPHA_ORDER;
    testinator::Results rs = r.RunAllTests(params, op.get());

    static string expected = "Bail out! Hello world 42";
    return !myTestB.m_runCalled
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestRunsInternal : public testinator::Test
{
public:
  TestRunsInternal(testinator::TestRegistry& r,
                   const string& name, const string& suite)
    : testinator::Test(r, name, suite)
  {}

  virtual bool Run()
  {
    return true;
  }
};

//------------------------------------------------------------------------------
class TestRuns : public testinator::Test
{
public:
  TestRuns(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    {
      testinator::TestRegistry r;
      TestRunsInternal myTestA(r, "A", "B");
      testinator::Results rs = r.RunAllTests();
      if (rs.empty() || !rs.front().m_success)
        return false;
    }
    {
      testinator::TestRegistry r;
      TestRunsInternal myTestA(r, "A", "B");
      testinator::Results rs = r.RunTest("A");
      if (rs.empty() || !rs.front().m_success)
        return false;
    }
    {
      testinator::TestRegistry r;
      TestRunsInternal myTestA(r, "A", "B");
      testinator::Results rs = r.RunSuite("B");
      if (rs.empty() || !rs.front().m_success)
        return false;
    }
    return true;
  }
};

//------------------------------------------------------------------------------
class TestNoSuchTest : public testinator::Test
{
public:
  TestNoSuchTest(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    testinator::Results rs = r.RunTest("A");
    return rs.empty();
  }
};

//------------------------------------------------------------------------------
class TestSkipOnSetupFailInternal : public testinator::Test
{
public:
  TestSkipOnSetupFailInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  virtual bool Setup(const testinator::RunParams&)
  {
    return false;
  }
};

//------------------------------------------------------------------------------
class TestSkipOnSetupFail : public testinator::Test
{
public:
  TestSkipOnSetupFail(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  virtual bool Run()
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::TAPOutputter>(oss);

    TestSkipOnSetupFailInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected = "A # skip";
    return oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(Macro, Test)
{
  return true;
}

//------------------------------------------------------------------------------
DEF_TEST(NonConstMacros, Test)
{
  EXPECT(true);
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
  TestRunMultiple test2("TestRunMultiple");
  TestReportResults test3("TestReportResults");
  TestRunSuite test4("TestRunSuite");
  TestExpectMacro test5("TestExpectMacro");
  TestDiagnostic test6("TestDiagnostic");
  TestBranch test7("TestBranch");
  TestSkip test8("TestSkip");
  TestExpectEvalsOnce test9("TestExpectEvalsOnce");
  TestBranchNoName test10("TestBranchNoName");
  TestAbort test11("TestAbort");
  TestRuns test12("TestRuns");
  TestTAPSkip test13("TestTAPSkip");
  TestTAPDiagnostic test14("TestTAPDiagnostic");
  TestTAPAbort test15("TestTAPAbort");
  TestNoSuchTest test16("TestNoSuchTest");
  TestSkipOnSetupFail test17("TestSkipOnSetupFail");
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
  return static_cast<int>(total - numPassed);
}
