#include "test.h"
using namespace testpp;

#include <algorithm>
#include <ctime>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

using namespace std;

namespace
{
  //------------------------------------------------------------------------------
  class TestRegistry
  {
  public:
    void Register(Test* test, const string& testName, const string& suiteName);
    void Unregister(Test* test);

    Results RunAllTests(const RunParams& params);
    Results RunSuite(const string& suiteName, const RunParams& params);
    Results RunTest(const string& testName, const RunParams& params);

  private:
    // Map of all tests.
    using TestMap = map<string, Test*>;
    // Reverse mapping of tests to test names.
    using TestNameMap = map<Test*, string>;

    // Map of all tests by suite.
    using TestSuiteMap = multimap<string, Test*>;
    // Reverse mapping of tests to suite names.
    using TestSuiteNameMap = map<Test*, string>;

    Results RunTests(TestMap& m, const RunParams& params);
    Result RunTest(Test* test, const RunParams& params);

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
void TestRegistry::Register(Test* test,
                            const string& testName,
                            const string& suiteName)
{
  m_tests.insert(make_pair(testName, test));
  m_testNames.insert(make_pair(test, testName));

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
  auto range = m_testsBySuite.equal_range(suiteName);
  for (auto& i = range.first; i != range.second; ++i)
  {
    if (i->second == test)
    {
      m_testsBySuite.erase(i);
      break;
    }
  }
}

//------------------------------------------------------------------------------
Results TestRegistry::RunAllTests(const RunParams& params)
{
  TestMap localMap;
  localMap.swap(m_tests);
  Results rs = RunTests(localMap, params);
  m_tests.swap(localMap);
  return rs;
}

//------------------------------------------------------------------------------
Results TestRegistry::RunSuite(const string& suiteName, const RunParams& params)
{
  TestMap localMap;
  auto range = m_testsBySuite.equal_range(suiteName);
  for (auto& i = range.first; i != range.second; ++i)
  {
    localMap.insert(make_pair(m_testNames[i->second], i->second));
  }
  return RunTests(localMap, params);
}

//------------------------------------------------------------------------------
Results TestRegistry::RunTest(const string& testName, const RunParams& params)
{
  auto i = m_tests.find(testName);
  if (i == m_tests.end())
    return std::vector<Result>();

  TestMap localMap;
  localMap.insert(make_pair(testName, i->second));
  return RunTests(localMap, params);
}

//------------------------------------------------------------------------------
Results TestRegistry::RunTests(TestMap& localMap, const RunParams& params)
{
  // Make a vector of test names, shuffle them if necessary.
  vector<const string*> testNames;
  testNames.reserve(localMap.size());
  for (auto& i : localMap)
  {
    testNames.push_back(&i.first);
  }
  if (!(params.m_flags & testpp::ALPHA_ORDER))
  {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(testNames.begin(), testNames.end(), g);
  }

  // Run each test.
  Results rs;
  rs.reserve(testNames.size());
  for (auto& i : testNames)
  {
    Test* test = localMap[*i];
    rs.push_back(RunTest(test, params));
  }

  return rs;
}

//------------------------------------------------------------------------------
Result TestRegistry::RunTest(Test* test, const RunParams& params)
{
  Result r;
  if (test->Setup(params))
  {
    r = test->RunWrapper();
    test->Teardown();
  }
  return r;
}

//------------------------------------------------------------------------------
Test::Test(const string& name, const string& suite)
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
Result Test::RunWrapper()
{
  Result r;
  r.m_success = Run() && m_success;
  r.m_message = m_message;
  return r;
}

//------------------------------------------------------------------------------
namespace testpp
{
  //------------------------------------------------------------------------------
  Results RunAllTests(const RunParams& params)
  {
    return GetTestRegistry().RunAllTests(params);
  }

  //------------------------------------------------------------------------------
  Results RunSuite(const string& suiteName, const RunParams& params)
  {
    return GetTestRegistry().RunSuite(suiteName, params);
  }

  //------------------------------------------------------------------------------
  Results RunTest(const string& testName, const RunParams& params)
  {
    return GetTestRegistry().RunTest(testName, params);
  }
}
