#pragma once

#include "output.h"

#include <memory>
#include <string>
#include <vector>

namespace testpp
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
    bool m_success;
  };

  using Results = std::vector<Result>;

  //------------------------------------------------------------------------------
  class Test
  {
  public:
    Test(const std::string& name, const std::string& suiteName = std::string());
    virtual ~Test();

    virtual bool Setup(const RunParams&) { return true; }
    virtual void Teardown() {}
    virtual bool Run() { return true; }

    Result RunWrapper(const Outputter* outputter)
    {
      Result r;
      m_op = outputter;
      r.m_success = Run() && m_success;
      return r;
    }

    const std::string& name() const { return m_name; }

  protected:
    bool m_success = true;
    std::string m_name;
    std::string m_message;
    const Outputter* m_op;
  };
}

//------------------------------------------------------------------------------
#define DECLARE_TEST(NAME, SUITE)                         \
  class SUITE##NAME : public testpp::Test                 \
  {                                                       \
  public:                                                 \
    SUITE##NAME()                                         \
      : testpp::Test(#NAME, #SUITE) {}                    \
    virtual bool Run() override;                          \
  } s_##SUITE##NAME##_Test;                               \
  bool SUITE##NAME::Run()

//------------------------------------------------------------------------------
#include "test_registry.h"

namespace testpp
{
  //------------------------------------------------------------------------------
  inline Results RunAllTests(const RunParams& params = RunParams(),
                             const Outputter* outputter = nullptr)
  {
    return GetTestRegistry().RunAllTests(
        params,
        outputter != nullptr ? outputter : std::make_unique<Outputter>().get());
  }

  inline Results RunSuite(const std::string& suiteName,
                          const RunParams& params = RunParams(),
                          const Outputter* outputter = nullptr)
  {
    return GetTestRegistry().RunSuite(
        suiteName,
        params,
        outputter != nullptr ? outputter : std::make_unique<Outputter>().get());
  }

  inline Results RunTest(const std::string& testName,
                         const RunParams& params = RunParams(),
                         const Outputter* outputter = nullptr)
  {
    return GetTestRegistry().RunTest(
        testName,
        params,
        outputter != nullptr ? outputter : std::make_unique<Outputter>().get());
  }

  //------------------------------------------------------------------------------
  inline Test::Test(const std::string& name, const std::string& suite)
    : m_name(name)
  {
    GetTestRegistry().Register(this, name, suite);
  }

  inline Test::~Test()
  {
    GetTestRegistry().Unregister(this);
  }
}
