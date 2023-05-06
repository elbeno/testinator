// Copyright (c) 2014-2016 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "output.h"

#include <memory>
#include <string>
#include <vector>

namespace testinator
{
  //------------------------------------------------------------------------------
  enum RunFlags : uint32_t
  {
    RF_NONE = 0,

    // ALPHA_ORDER means run tests in alphabetical order (default is random
    // order).
    RF_ALPHA_ORDER = 1 << 0,
  };

  //------------------------------------------------------------------------------
  struct RunParams
  {
    uint32_t m_flags = RF_NONE;
    size_t m_numPropertyChecks = 100;
    unsigned long m_randomSeed = 0;
  };

  //------------------------------------------------------------------------------
  struct Result
  {
    std::string m_suiteName;
    std::string m_testName;
    bool m_success{};
  };

  using Results = std::vector<Result>;

  //------------------------------------------------------------------------------
  class TestRegistry;

  class Test
  {
  public:
    Test(TestRegistry& reg, const std::string& name,
         const std::string& suiteName = std::string());
    explicit Test(const std::string& name, const std::string& suiteName = std::string());
    virtual ~Test();

    virtual bool Setup(const RunParams&) { return true; }
    virtual bool Run() { return true; }
    bool RunWithBranches();

    Result RunWrapper(const Outputter* outputter)
    {
      Result r;
      m_op = outputter;
      r.m_success = RunWithBranches() && m_success;
      return r;
    }

    const std::string& GetName() const { return m_name; }
    bool skipped() const { return m_skipped; }

  protected:
    bool m_success = true;
    bool m_skipped = false;
    const std::string m_name;
    TestRegistry& m_registry;
    const Outputter* m_op{};
  };
}

//------------------------------------------------------------------------------
#define DEF_TEST(NAME, SUITE)                             \
  class SUITE##NAME : public testinator::Test             \
  {                                                       \
  public:                                                 \
    SUITE##NAME()                                         \
      : testinator::Test(#NAME, #SUITE) {}                \
    virtual bool Run() override;                          \
  } s_##SUITE##NAME##_Test;                               \
  bool SUITE##NAME::Run()

//------------------------------------------------------------------------------
#include "branch.h"
#include "test_registry.h"

namespace testinator
{
  //------------------------------------------------------------------------------
  inline Results RunAllTests(const RunParams& params = RunParams(),
                             const Outputter* outputter = nullptr)
  {
    return GetTestRegistry().RunAllTests(params, outputter);
  }

  inline Results RunSuite(const std::string& suiteName,
                          const RunParams& params = RunParams(),
                          const Outputter* outputter = nullptr)
  {
    return GetTestRegistry().RunSuite(suiteName, params, outputter);
  }

  inline Results RunTest(const std::string& testName,
                         const RunParams& params = RunParams(),
                         const Outputter* outputter = nullptr)
  {
    return GetTestRegistry().RunTest(testName, params, outputter);
  }

  //------------------------------------------------------------------------------
  inline Test::Test(TestRegistry& r, const std::string& n, const std::string& s)
    : m_name(n)
    , m_registry(r)
  {
    m_registry.Register(this, n, s);
  }

  inline Test::Test(const std::string& n, const std::string& s)
    : Test(GetTestRegistry(), n, s)
  {
  }

  inline Test::~Test()
  {
    m_registry.Unregister(this);
  }
}
