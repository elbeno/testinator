#pragma once

#include <iostream>

namespace testpp
{
  //------------------------------------------------------------------------------
  struct DefaultOutputter
  {
    void startRun(int)
    {
    }

    void skipTest(const char*, const char*)
    {
    }

    void startTest(const char*)
    {
    }

    void diagnostic(const char*)
    {
    }

    void endTest(const char*, bool success)
    {
      ++m_numTests;
      if (success) ++m_numSuccesses;
    }

    void abort(const char*)
    {
    }

    void endRun()
    {
      std::cout << m_numSuccesses << '/' << m_numTests
                << " tests passed." << std::endl;
    }

    int m_numTests = 0;
    int m_numSuccesses = 0;
  };

  //------------------------------------------------------------------------------
  struct VerboseOutputter
  {
    void startRun(int)
    {
    }

    void skipTest(const char*, const char*)
    {
    }

    void startTest(const char*)
    {
    }

    void diagnostic(const char*)
    {
    }

    void endTest(const char*, bool)
    {
    }

    void abort(const char*)
    {
    }

    void endRun()
    {
    }
  };

  //------------------------------------------------------------------------------
  struct TAPOutputter
  {
    void startRun(int num_tests)
    {
      m_numTests = 0;
      std::cout << "1.." << num_tests << std::endl;
    }

    void skipTest(const char* name, const char* msg)
    {
      std::cout << "ok "
                << ++m_numTests
                << ' ' << name
                << " # skip " << msg << std::endl;
    }

    void startTest(const char*)
    {
    }

    void diagnostic(const char* msg)
    {
      std::cout << "# " << msg << std::endl;
    }

    void endTest(const char* name, bool success)
    {
      std::cout << (success ? "ok " : "not ok ")
                << ++m_numTests
                << ' ' << name << std::endl;
    }

    void abort(const char* msg)
    {
      std::cout << "Bail out! " << msg << std::endl;
    }

    void endRun()
    {
    }

    int m_numTests = 0;
  };

}
