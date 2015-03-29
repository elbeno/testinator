#pragma once

#include "test.h"
#include <sstream>

namespace testpp
{
  inline std::string FailureMessage(
      const char* type,
      const char* failureText,
      const char* file,
      int line)
  {
    std::ostringstream s;
    s << type << " FAILED: "
      << file << ":" << line
      << " (" << failureText << ")";
    return s.str();
  }
}

#define EXPECT_AUX(type, cond, file, line)                              \
  if (!(cond))                                                          \
  {                                                                     \
    m_op->diagnostic(                                                   \
        testpp::FailureMessage(type, #cond, file, line));               \
    m_success = false;                                                  \
  }
#define EXPECT(cond) EXPECT_AUX("EXPECT", cond, __FILE__, __LINE__)

#define EXPECT_NOT_AUX(type, cond, file, line)                          \
  if (cond)                                                             \
  {                                                                     \
    m_op->diagnostic(                                                   \
        testpp::FailureMessage(type, #cond, file, line));               \
    m_success = false;                                                  \
  }
#define EXPECT_NOT(cond) EXPECT_NOT_AUX("EXPECT_NOT", cond, __FILE__, __LINE__)
