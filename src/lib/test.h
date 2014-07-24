#pragma once

#include <ostream>

namespace testpp
{
  //------------------------------------------------------------------------------
  class Test
  {
  public:
    Test(const char* name, const char* suite = "");
    virtual ~Test();

    virtual bool Setup() { return true; }
    virtual void Teardown() {}
    virtual bool Run() { return true; }

    virtual bool RunWrapper()
    {
      return Run() && m_success;
    }

  protected:
    bool m_success;
  };

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
  struct Results
  {
    int m_numPassed;
    int m_numFailed;
  };

  //------------------------------------------------------------------------------
  void RunAllTests(Results& results, RunFlags flags = COLOR);
  void RunSuite(const char* suiteName, Results& results, RunFlags flags = COLOR);

  bool Run(const char* testName);

  std::ostream& GetStream();
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
