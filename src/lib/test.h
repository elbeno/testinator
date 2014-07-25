#pragma once

#include <ostream>

namespace testpp
{
  //------------------------------------------------------------------------------
  enum RunFlags
  {
    NONE = 0,

    // QUIET means no output.
    QUIET = 1 << 0,

    // COLOR means use ANSI color coded output: red for fail, green for pass.
    COLOR = 1 << 1,

    // ALPHA_ORDER means run tests in alphabetical order (default is random
    // order).
    ALPHA_ORDER = 1 << 2,

    // QUIET_SUCCESS means output for failing tests only.
    QUIET_SUCCESS = 1 << 3,
  };

  //------------------------------------------------------------------------------
  struct RunParams
  {
    RunParams() : m_flags(COLOR | QUIET_SUCCESS), m_numPropertyChecks(100) {}
    unsigned int m_flags;
    size_t m_numPropertyChecks;
  };

  //------------------------------------------------------------------------------
  struct Results
  {
    int m_numPassed;
    int m_numFailed;
  };

  //------------------------------------------------------------------------------
  void RunAllTests(Results& results, const RunParams& params = RunParams());
  void RunSuite(const char* suiteName, Results& results, const RunParams& params = RunParams());

  bool Run(const char* testName, const testpp::RunParams& params = RunParams());

  std::ostream& GetStream();

  //------------------------------------------------------------------------------
  class Test
  {
  public:
    Test(const char* name, const char* suite = "");
    virtual ~Test();

    virtual bool Setup(const testpp::RunParams&) { return true; }
    virtual void Teardown() {}
    virtual bool Run() { return true; }

    virtual bool RunWrapper()
    {
      return Run() && m_success;
    }

  protected:
    bool m_success;
  };
}

//------------------------------------------------------------------------------
#define DECLARE_TEST(NAME, SUITE)               \
  class SUITE##NAME : public testpp::Test       \
  {                                             \
  public:                                       \
    SUITE##NAME()                               \
      : testpp::Test(#NAME, #SUITE) {}          \
    virtual bool Run();                         \
  } s_##SUITE##NAME##_Test;                     \
  bool SUITE##NAME::Run()

//------------------------------------------------------------------------------
#include "test_macros.h"
