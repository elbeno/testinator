#pragma once

#include "test.h"

namespace testpp
{
  inline void OutputFailure(
      const char* type,
      const char* failureText,
      const char* file,
      int line)
  {
    std::ostream& s = testpp::GetStream();
    s << type << " FAILED: "
      << file << ":" << line
      << " (" << failureText << ")" << std::endl;
  }
}

#define EXPECT_AUX(type, cond, file, line)              \
  if (!(cond))                                          \
  {                                                     \
    testpp::OutputFailure(type, #cond, file, line);       \
    m_success = false;                                  \
  }
#define EXPECT(cond) EXPECT_AUX("EXPECT", cond, __FILE__, __LINE__)

#define EXPECT_NOT_AUX(type, cond, file, line)          \
  if (cond)                                             \
  {                                                     \
    testpp::OutputFailure(type, #cond, file, line);       \
    m_success = false;                                  \
  }
#define EXPECT_NOT(cond) EXPECT_NOT_AUX("EXPECT_NOT", cond, __FILE__, __LINE__)
