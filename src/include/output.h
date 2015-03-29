#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace testpp
{
#define RED "\033[31;1m"
#define GREEN "\033[32;1m"
#define NORMAL "\033[0m"
#define COLORIZE(C, S) ((m_flags & OF_COLOR) ? (C S NORMAL) : S)

  enum OutputFlags : uint32_t
  {
    OF_NONE = 0,
    OF_COLOR = 1 << 0,
    OF_QUIET_SUCCESS = 1 << 1
  };

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
    DefaultOutputter(std::ostream& os = std::cout,
                     OutputFlags flags = OF_COLOR)
      : m_os(os)
      , m_flags(flags)
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
        m_os << COLORIZE(RED, "FAIL") << ": " << name << std::endl;
      }
      else
      {
        if (!(m_flags & OF_QUIET_SUCCESS))
        {
          m_os << COLORIZE(GREEN, "PASS") << ": " << name << std::endl;
        }
      }
    }

    virtual void abort(const std::string& msg) const override
    {
      m_os << COLORIZE(RED, "ABORT") << " (" << msg << ')' << std::endl;
    }

    virtual void endRun(int numTests, int numSuccesses) const override
    {
      m_os << numSuccesses << '/' << numTests
           << " tests passed." << std::endl;
    }

  private:
    std::ostream& m_os;
    OutputFlags m_flags;
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

#undef COLORIZE
#undef RED
#undef GREEN
#undef NORMAL

  //------------------------------------------------------------------------------
  inline std::unique_ptr<Outputter> MakeOutputter(
      const std::string& name,
      OutputFlags flags)
  {
    std::unique_ptr<Outputter> op;
    if (name == "TAP")
      op = std::make_unique<testpp::TAPOutputter>();
    else
      op = std::make_unique<testpp::DefaultOutputter>(std::cout, flags);
    return op;
  }

}
