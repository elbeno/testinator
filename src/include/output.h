#pragma once

#include <iostream>
#include <string>

namespace testpp
{
  static constexpr const char* const RED = "\033[31;1m";
  static constexpr const char* const GREEN = "\033[32;1m";
  static constexpr const char* const NORMAL = "\033[0m";

  struct Outputter
  {
    virtual void startRun(int) const {};
    virtual void skipTest(const std::string&, const std::string&) const {};
    virtual void startTest(const std::string&) const {};
    virtual void diagnostic(const std::string&) const {};
    virtual void endTest(const std::string&, bool) const {};
    virtual void abort(const std::string&) const {};
    virtual void endRun(int, int) const {};
  };

  //------------------------------------------------------------------------------
  struct DefaultOutputter : public Outputter
  {
    DefaultOutputter(std::ostream& os = std::cout)
      : m_os(os)
    {}

    virtual void startRun(int) const override
    {
    }

    virtual void skipTest(const std::string&, const std::string&) const override
    {
    }

    virtual void startTest(const std::string&) const override
    {
    }

    virtual void diagnostic(const std::string& msg) const override
    {
      m_os << msg << std::endl;
    }

    virtual void endTest(const std::string& name, bool success) const override
    {
      if (!success)
      {
        m_os << RED << "FAIL" << NORMAL
             << ": " << name << std::endl;
      }
    }

    virtual void abort(const std::string& msg) const override
    {
      m_os << "Aborted (" << msg << ')' << std::endl;
    }

    virtual void endRun(int numTests, int numSuccesses) const override
    {
      m_os << numSuccesses << '/' << numTests
           << " tests passed." << std::endl;
    }

    std::ostream& m_os;
  };

  //------------------------------------------------------------------------------
  struct TAPOutputter : public Outputter
  {
    virtual void startRun(int numTests) const override
    {
      m_numTests = 0;
      std::cout << "1.." << numTests << std::endl;
    }

    virtual void skipTest(const std::string& name, const std::string& msg) const override
    {
      std::cout << "ok "
                << ++m_numTests
                << ' ' << name
                << " # skip " << msg << std::endl;
    }

    virtual void startTest(const std::string&) const override
    {
    }

    virtual void diagnostic(const std::string& msg) const override
    {
      std::cout << "# " << msg << std::endl;
    }

    virtual void endTest(const std::string& name, bool success) const override
    {
      std::cout << (success ? "ok " : "not ok ")
                << ++m_numTests
                << ' ' << name << std::endl;
    }

    virtual void abort(const std::string& msg) const override
    {
      std::cout << "Bail out! " << msg << std::endl;
    }

    virtual void endRun(int, int) const override
    {
    }

    mutable int m_numTests;
  };

}
