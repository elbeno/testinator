#pragma once

#include "test.h"

namespace testpp
{
  std::ostream& RunAllTests(
      Results& results, const RunParams& params, std::ostream& stream);

  std::ostream& RunSuite(
      const char* suite,
      Results& results, const RunParams& params, std::ostream& stream);
}
