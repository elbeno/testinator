// Copyright (c) 2014-2016 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#include <testinator.h>

#include <algorithm>
#include <ios>
#include <memory>
#include <sstream>
#include <string>

using namespace std;

//------------------------------------------------------------------------------
namespace
{
  const char* s_suiteName = "Test";
}

//------------------------------------------------------------------------------
class TestCallTest : public testinator::Test
{
public:
  explicit TestCallTest(const string& name) : testinator::Test(name, s_suiteName) {}

  bool Run() override
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
  explicit TestSetupFirst(const string& name)
    : testinator::Test(name, s_suiteName)
    , m_setupCalled(false)
  {}

  bool Setup(const testinator::RunParams&) override
  {
    m_setupCalled = true;
    return true;
  }

  bool Run() override
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

  bool Run() override
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
  explicit TestRunMultiple(const std::string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Run() override
  {
    return !m_fail;
  }

  bool m_fail;
};

//------------------------------------------------------------------------------
class TestReportResults : public testinator::Test
{
public:
  explicit TestReportResults(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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
  explicit TestRunSuite(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Run() override
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
  explicit TestExpectMacro(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestExpectMacroInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected =
      "main.cpp:184 (!fail == fail => false == true)";
    return !rs.empty() && !rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
struct CountEvals
{
  static bool inc() { ++s_evals; return true; }
  static int s_evals;
};
int CountEvals::s_evals = 0;

class TestExpectEvalsOnceInternal : public testinator::Test
{
public:
  TestExpectEvalsOnceInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  bool Run() override
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
  explicit TestExpectEvalsOnce(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Run() override
  {
    DIAGNOSTIC("Hello world " << 42 << endl << 42);
    DIAGNOSTIC("Hello world " << std::hex << 42);
    return true;
  }
};

//------------------------------------------------------------------------------
class TestDiagnostic : public testinator::Test
{
public:
  explicit TestDiagnostic(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
  {
    testinator::TestRegistry r;
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);

    TestDiagnosticInternal myTestA(r, "A");
    testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

    static string expected1 = "Hello world 42\n42";
    static string expected2 = "Hello world 2a";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected1) != string::npos
      && oss.str().find(expected2) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestAbortInternal : public testinator::Test
{
public:
  TestAbortInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  bool Run() override
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
  explicit TestAbort(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Run() override
  {
    SKIP("Hello world " << 42);
    return false;
  }
};

//------------------------------------------------------------------------------
class TestSkip : public testinator::Test
{
public:
  explicit TestSkip(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Run() override
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
  explicit TestBranch(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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
  explicit TestBranchInternal2(const string& name)
    : testinator::Test(name)
  {}

  bool Run() override
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
  explicit TestBranchNoName(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
  {
    ostringstream oss;
    std::unique_ptr<testinator::Outputter> op =
      make_unique<testinator::DefaultOutputter>(oss);
    TestBranchInternal2 myTestA("A");
    testinator::Results rs = testinator::RunAllTests(testinator::RunParams(), op.get());

    static string expected = "main.cpp:448";
    return !rs.empty() && rs.front().m_success
      && oss.str().find(expected) != string::npos;
  }
};

//------------------------------------------------------------------------------
class TestTAPSkip : public testinator::Test
{
public:
  explicit TestTAPSkip(const string& name)
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
  explicit TestTAPDiagnostic(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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
  explicit TestTAPAbort(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Run() override
  {
    return true;
  }
};

//------------------------------------------------------------------------------
class TestRuns : public testinator::Test
{
public:
  explicit TestRuns(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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
  explicit TestNoSuchTest(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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

  bool Setup(const testinator::RunParams&) override
  {
    return false;
  }
};

//------------------------------------------------------------------------------
class TestSkipOnSetupFail : public testinator::Test
{
public:
  explicit TestSkipOnSetupFail(const string& name)
    : testinator::Test(name, s_suiteName)
  {}

  bool Run() override
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
class TestETCaptureCoverageInternal : public testinator::Test
{
public:
  TestETCaptureCoverageInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  bool Run() override
  {
    EXPECT(1+2 > 1+3 < 0);
    EXPECT(false);
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(ETCaptureCoverage, Test)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestETCaptureCoverageInternal myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "EXPECT FAILED";
  return oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class TestPrettyPrintCoverageInternal : public testinator::Test
{
public:
  TestPrettyPrintCoverageInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  bool Run() override
  {
    vector<int> v = {1,2,3};
    DIAGNOSTIC(testinator::prettyprint(v));
    string s{"Hello"};
    DIAGNOSTIC(testinator::prettyprint(s));
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(PrettyPrintCoverage, Test)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestPrettyPrintCoverageInternal myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected1 = "[1,2,3]";
  static string expected2 = "\"Hello\"";
  return oss.str().find(expected1) != string::npos
    && oss.str().find(expected2) != string::npos;
}

//------------------------------------------------------------------------------
class TestComplexityCoverageInternal : public testinator::Test
{
public:
  TestComplexityCoverageInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  bool Run() override
  {
    DIAGNOSTIC(testinator::ComplexityProperty::Order(testinator::ORDER_1));
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(ComplexityCoverage, Test)
{
  testinator::TestRegistry r;
  ostringstream oss;
  std::unique_ptr<testinator::Outputter> op =
    make_unique<testinator::DefaultOutputter>(oss);

  TestComplexityCoverageInternal myTestA(r, "A");
  testinator::Results rs = r.RunAllTests(testinator::RunParams(), op.get());

  static string expected = "O(1)";
  return oss.str().find(expected) != string::npos;
}

//------------------------------------------------------------------------------
class TestOutputCoverageInternal : public testinator::Test
{
public:
  TestOutputCoverageInternal(testinator::TestRegistry& r, const string& name)
    : testinator::Test(r, name)
  {}

  bool Run() override
  {
    DIAGNOSTIC("Foo");
    SKIP("Foo");
    ABORT("Foo");
    return true;
  }
};

//------------------------------------------------------------------------------
DEF_TEST(OutputCoverage, Test)
{
  testinator::TestRegistry r;
  std::unique_ptr<testinator::Outputter> op =
    testinator::MakeOutputter("TAP", testinator::OF_NONE);
  TestOutputCoverageInternal myTestA(r, "A");
  testinator::Results rs = r.RunAllTests();
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
DEF_TEST(ArbitraryCoverage, Test)
{
  struct MyStruct {} s;
  auto v = testinator::Arbitrary<MyStruct>::shrink(s);
  return v.empty();
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
