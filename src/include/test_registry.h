#include "output.h"

#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <vector>

namespace testpp
{
  //------------------------------------------------------------------------------
  class TestRegistry
  {
  public:

    static TestRegistry& Instance()
    {
      static TestRegistry s_testRegistry;
      return s_testRegistry;
    }

    //------------------------------------------------------------------------------
    void Register(Test* test,
                  const std::string& testName,
                  const std::string& suiteName)
    {
      m_tests.insert({testName, test});
      m_testNames.insert({test, testName});

      m_testsBySuite.insert({suiteName, test});
      m_suiteNames.insert({test, suiteName});

      std::random_device rd;
      m_generator.seed(rd());
    }

    //------------------------------------------------------------------------------
    void Unregister(Test* test)
    {
      const std::string& testName = m_testNames[test];
      m_testNames.erase(test);
      m_tests.erase(testName);

      const std::string& suiteName = m_suiteNames[test];
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
    Results RunAllTests(const RunParams& params)
    {
      TestMap localMap;
      localMap.swap(m_tests);
      Results rs = RunTests(localMap, params);
      m_tests.swap(localMap);
      return rs;
    }

    Results RunSuite(const std::string& suiteName, const RunParams& params)
    {
      TestMap localMap;
      auto range = m_testsBySuite.equal_range(suiteName);
      for (auto& i = range.first; i != range.second; ++i)
      {
        localMap.insert({m_testNames[i->second], i->second});
      }
      return RunTests(localMap, params);
    }

    Results RunTest(const std::string& testName, const RunParams& params)
    {
      auto i = m_tests.find(testName);
      if (i == m_tests.end())
        return std::vector<Result>();

      TestMap localMap;
      localMap.insert({testName, i->second});
      return RunTests(localMap, params);
    }

    std::mt19937& RNG() { return m_generator; }
    const Outputter& GetOutputter() const { return *m_outputter; }

  private:
    // Map of all tests.
    using TestMap = std::map<std::string, Test*>;
    // Reverse mapping of tests to test names.
    using TestNameMap = std::map<Test*, std::string>;

    // Map of all tests by suite.
    using TestSuiteMap = std::multimap<std::string, Test*>;
    // Reverse mapping of tests to suite names.
    using TestSuiteNameMap = std::map<Test*, std::string>;

    Results RunTests(TestMap& m, const RunParams& params)
    {
      params.m_outputter.startRun(m.size());
      int numSuccesses = 0;

      // Make a vector of test names, shuffle them if necessary.
      std::vector<const std::string*> testNames;
      testNames.reserve(m.size());
      for (auto& i : m)
      {
        testNames.push_back(&i.first);
      }
      if (!(params.m_flags & testpp::ALPHA_ORDER))
      {
        std::shuffle(testNames.begin(), testNames.end(), m_generator);
      }

      // Run each test.
      Results rs;
      rs.reserve(testNames.size());
      for (auto& i : testNames)
      {
        Test* test = m[*i];
        Result r = RunTest(test, params);
        if (r.m_success) ++numSuccesses;
        rs.push_back(std::move(r));
      }

      params.m_outputter.endRun(m.size(), numSuccesses);
      return rs;
    }

    Result RunTest(Test* test, const RunParams& params)
    {
      m_outputter = &params.m_outputter;
      Result r;
      if (test->Setup(params))
      {
        params.m_outputter.startTest(test->name());
        r = test->RunWrapper();
        params.m_outputter.endTest(test->name(), test->success());
        test->Teardown();
      }
      else
      {
        m_outputter->skipTest(test->name(), std::string());
      }
      return r;
    }

    TestMap m_tests;
    TestNameMap m_testNames;
    TestSuiteMap m_testsBySuite;
    TestSuiteNameMap m_suiteNames;

    std::mt19937 m_generator;
    const Outputter* m_outputter;
  };

  //------------------------------------------------------------------------------
  inline TestRegistry& GetTestRegistry()
  {
    return TestRegistry::Instance();
  }
}
