// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "output.h"

#include <algorithm>
#include <map>
#include <random>
#include <string>
#include <vector>

namespace testinator
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
      m_tests.erase(testName);
      m_testNames.erase(test);

      const std::string& suiteName = m_suiteNames[test];
      auto range = m_testsBySuite.equal_range(suiteName);
      for (auto& i = range.first; i != range.second; ++i)
      {
        if (i->second == test)
        {
          m_testsBySuite.erase(i);
          break;
        }
      }
      m_suiteNames.erase(test);
    }

    //------------------------------------------------------------------------------
    Results RunAllTests(const RunParams& params = RunParams(),
                        const Outputter* outputter = nullptr)
    {
      TestMap localMap;
      localMap.swap(m_tests);
      Results rs = RunTests(
          localMap,
          params,
          outputter != nullptr ? outputter : std::make_unique<Outputter>().get());
      m_tests.swap(localMap);
      return rs;
    }

    Results RunSuite(const std::string& suiteName,
                     const RunParams& params = RunParams(),
                     const Outputter* outputter = nullptr)
    {
      TestMap localMap;
      auto range = m_testsBySuite.equal_range(suiteName);
      for (auto& i = range.first; i != range.second; ++i)
      {
        localMap.insert({m_testNames[i->second], i->second});
      }
      return RunTests(
          localMap,
          params,
          outputter != nullptr ? outputter : std::make_unique<Outputter>().get());
    }

    Results RunTest(const std::string& testName,
                    const RunParams& params = RunParams(),
                    const Outputter* outputter = nullptr)
    {
      auto i = m_tests.find(testName);
      if (i == m_tests.end())
        return std::vector<Result>();

      TestMap localMap;
      localMap.insert({testName, i->second});
      return RunTests(
          localMap,
          params,
          outputter != nullptr ? outputter : std::make_unique<Outputter>().get());
    }

    std::mt19937& RNG() { return m_generator; }
    void Abort() { m_abort = true; }

  private:
    // Map of all tests.
    using TestMap = std::map<std::string, Test*>;
    // Reverse mapping of tests to test names.
    using TestNameMap = std::map<Test*, std::string>;

    // Map of all tests by suite.
    using TestSuiteMap = std::multimap<std::string, Test*>;
    // Reverse mapping of tests to suite names.
    using TestSuiteNameMap = std::map<Test*, std::string>;

    Results RunTests(TestMap& m,
                     const RunParams& params,
                     const Outputter* outputter)
    {
      outputter->startRun(m.size());
      std::size_t numSuccesses = 0;

      // Make a vector of test names, shuffle them if necessary.
      std::vector<const std::string*> testNames;
      testNames.reserve(m.size());
      for (auto& i : m)
      {
        testNames.push_back(&i.first);
      }
      if (!(params.m_flags & RF_ALPHA_ORDER))
      {
        std::shuffle(testNames.begin(), testNames.end(), m_generator);
      }

      // Run each test.
      Results rs;
      rs.reserve(testNames.size());
      for (auto& i : testNames)
      {
        Test* test = m[*i];
        Result r = RunTest(test, params, outputter);
        if (r.m_success) ++numSuccesses;
        rs.push_back(std::move(r));
        if (m_abort)
        {
          m_abort = false;
          break;
        }
      }

      outputter->endRun(m.size(), numSuccesses);
      return rs;
    }

    Result RunTest(Test* test,
                   const RunParams& params,
                   const Outputter* outputter)
    {
      Result r;
      if (test->Setup(params))
      {
        outputter->startTest(test->GetName());
        r = test->RunWrapper(outputter);
        if (!test->skipped())
          outputter->endTest(test->GetName(), r.m_success);
      }
      else
      {
        outputter->skipTest(test->GetName(), std::string());
      }
      return r;
    }

    TestMap m_tests;
    TestNameMap m_testNames;
    TestSuiteMap m_testsBySuite;
    TestSuiteNameMap m_suiteNames;

    std::mt19937 m_generator;
    bool m_abort = false;
  };

  //------------------------------------------------------------------------------
  inline TestRegistry& GetTestRegistry()
  {
    return TestRegistry::Instance();
  }
}
