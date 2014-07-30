#include <test_extended.h>

using namespace testpp;

#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace
{
  static const char* RED = "\033[31;1m";
  static const char* GREEN = "\033[32;1m";
  static const char* NORMAL = "\033[0m";

  static ostream* s_stream = 0;

  //------------------------------------------------------------------------------
  class TestRegistry
  {
  public:
    TestRegistry();

    void Register(Test* test, const char* name, const char* suite);
    void Unregister(Test* test);

    ostream& RunAllTests(
        testpp::Results& results, const testpp::RunParams& params, ostream& stream);

    ostream& RunSuite(
        const char* suiteName,
        testpp::Results& results, const testpp::RunParams& params, ostream& stream);

    ostream& RunTest(
        const char* testName,
        testpp::Results& results, const testpp::RunParams& params, ostream& stream);

    bool RunTest(Test* test, const testpp::RunParams& params);
    bool RunTest(const char* testName, const testpp::RunParams& params);

  private:
    // Map of all tests.
    typedef map<string, Test*> TestMap;
    // Reverse mapping of tests to test names.
    typedef map<Test*, string> TestNameMap;

    // Map of all tests by suite.
    typedef multimap<string, Test*> TestSuiteMap;
    // Reverse mapping of tests to suite names.
    typedef map<Test*, string> TestSuiteNameMap;

    void RunTests(
        TestMap& m, testpp::Results& results, const testpp::RunParams& params, ostream& stream);

    TestMap m_tests;
    TestNameMap m_testNames;
    TestSuiteMap m_testsBySuite;
    TestSuiteNameMap m_suiteNames;
  };

  //------------------------------------------------------------------------------
  TestRegistry& GetTestRegistry()
  {
    static TestRegistry s_testRegistry;
    return s_testRegistry;
  }
}

//------------------------------------------------------------------------------
TestRegistry::TestRegistry()
{
  // Randomize things at construction time.
  srand(time(0));
}

//------------------------------------------------------------------------------
void TestRegistry::Register(Test* test, const char* name, const char* suite)
{
  string testName(name);
  m_tests.insert(make_pair(testName, test));
  m_testNames.insert(make_pair(test, testName));

  string suiteName(suite);
  m_testsBySuite.insert(make_pair(suiteName, test));
  m_suiteNames.insert(make_pair(test, suiteName));
}

//------------------------------------------------------------------------------
void TestRegistry::Unregister(Test* test)
{
  const string& testName = m_testNames[test];
  m_testNames.erase(test);
  m_tests.erase(testName);

  const string& suiteName = m_suiteNames[test];
  m_suiteNames.erase(test);
  pair<TestSuiteMap::iterator, TestSuiteMap::iterator> range =
    m_testsBySuite.equal_range(suiteName);
  for (TestSuiteMap::iterator i = range.first;
       i != range.second;
       ++i)
  {
    if (i->second == test)
    {
      m_testsBySuite.erase(i);
      break;
    }
  }
}

//------------------------------------------------------------------------------
ostream& TestRegistry::RunAllTests(
    testpp::Results& results, const testpp::RunParams& params, ostream& stream)
{
  TestMap localMap;
  localMap.swap(m_tests);

  RunTests(localMap, results, params, stream);

  m_tests.swap(localMap);
  return stream;
}

//------------------------------------------------------------------------------
ostream& TestRegistry::RunSuite(
    const char* suiteName,
    testpp::Results& results, const testpp::RunParams& params, ostream& stream)
{
  TestMap localMap;

  pair<TestSuiteMap::iterator, TestSuiteMap::iterator> range =
    m_testsBySuite.equal_range(suiteName);

  for (auto i = range.first; i != range.second; ++i)
  {
    localMap.insert(make_pair(m_testNames[i->second], i->second));
  }

  RunTests(localMap, results, params, stream);

  return stream;
}

//------------------------------------------------------------------------------
ostream& TestRegistry::RunTest(
    const char* testName,
    testpp::Results& results, const testpp::RunParams& params, ostream& stream)
{
  TestMap::iterator i = m_tests.find(testName);
  if (i == m_tests.end())
    return stream;

  TestMap localMap;
  localMap.insert(make_pair(testName, i->second));

  RunTests(localMap, results, params, stream);

  return stream;
}

//------------------------------------------------------------------------------
void TestRegistry::RunTests(
    TestMap& localMap,
    testpp::Results& results, const testpp::RunParams& params, ostream& stream)
{
  s_stream = &stream;

  results.m_numPassed = results.m_numFailed = 0;

  // Make a vector of test names, shuffle them if necessary.
  vector<const string*> testNames;
  testNames.reserve(localMap.size());
  for (TestMap::iterator i = localMap.begin();
       i != localMap.end();
       ++i)
  {
    testNames.push_back(&i->first);
  }
  if (!(params.m_flags & testpp::ALPHA_ORDER))
  {
    random_shuffle(
        testNames.begin(), testNames.end(),
        [] (int i) -> int { return rand() % i; });
  }

  // Run each test.
  for (vector<const string*>::const_iterator i = testNames.begin();
       i != testNames.end();
       ++i)
  {
    Test* test = localMap[**i];
    bool success = RunTest(test, params);
    if (!success || !(params.m_flags & testpp::QUIET_SUCCESS))
    {
      const char* colorCode = success ? GREEN : RED;
      const char* resultText = success ? "PASS" : "FAIL";
      if (params.m_flags & testpp::COLOR)
      {
        stream << colorCode;
      }
      stream << resultText;
      if (params.m_flags & testpp::COLOR)
      {
        stream << NORMAL;
      }
      stream << ": " << m_suiteNames[test]
             << "::" << **i << endl;
    }

    if (success)
    {
      ++results.m_numPassed;
    }
    else
    {
      ++results.m_numFailed;
    }
  }

  s_stream = 0;
}

//------------------------------------------------------------------------------
bool TestRegistry::RunTest(Test* test, const testpp::RunParams& params)
{
  bool success = false;
  if (test->Setup(params))
  {
    success = test->RunWrapper();
    test->Teardown();
  }
  return success;
}

//------------------------------------------------------------------------------
bool TestRegistry::RunTest(const char* testName, const testpp::RunParams& params)
{
  TestMap::iterator i = m_tests.find(testName);
  return i != m_tests.end() && RunTest(i->second, params);
}

//------------------------------------------------------------------------------
Test::Test(const char* name, const char* suite)
  : m_success(true)
  , m_name(name)
{
  GetTestRegistry().Register(this, name, suite);
}

//------------------------------------------------------------------------------
Test::~Test()
{
  GetTestRegistry().Unregister(this);
}

//------------------------------------------------------------------------------
void testpp::RunAllTests(
    testpp::Results& results, const testpp::RunParams& params)
{
  GetTestRegistry().RunAllTests(results, params, cout);
}

//------------------------------------------------------------------------------
ostream& testpp::RunAllTests(
    testpp::Results& results, const testpp::RunParams& params, ostream& stream)
{
  return GetTestRegistry().RunAllTests(results, params, stream);
}

//------------------------------------------------------------------------------
void testpp::RunSuite(
    const char* suite, testpp::Results& results, const testpp::RunParams& params)
{
  GetTestRegistry().RunSuite(suite, results, params, cout);
}

//------------------------------------------------------------------------------
void testpp::RunTest(
    const char* test, testpp::Results& results, const testpp::RunParams& params)
{
  GetTestRegistry().RunTest(test, results, params, cout);
}

//------------------------------------------------------------------------------
ostream& testpp::RunSuite(
    const char* suite,
    testpp::Results& results, const testpp::RunParams& params, ostream& stream)
{
  return GetTestRegistry().RunSuite(suite, results, params, stream);
}

//------------------------------------------------------------------------------
bool testpp::Run(const char* testName, const testpp::RunParams& params)
{
  return GetTestRegistry().RunTest(testName, params);
}

//------------------------------------------------------------------------------
ostream& testpp::GetStream()
{
  return *s_stream;
}
