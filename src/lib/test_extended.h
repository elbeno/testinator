#pragma once

#include "test.h"

namespace testpp
{
  std::ostream& RunAllTests(
      Results& results, RunFlags flags, std::ostream& stream);

  std::ostream& RunSuite(
      const char* suite,
      Results& results, RunFlags flags, std::ostream& stream);
}
