# TestPP

Experiments with testing in C++. TestPP is a unit testing framework partially
inspired by Haskell's QuickCheck.

## Simple Tests

To use ordinary tests like any other testing framework, `#include <test.h>`.

Ordinary unit tests are grouped into suites and declared with a macro
(arguments: test name and suite name) and function body which should return
`true` if the test is successful, `false` otherwise.

```cpp
DECLARE_TEST(TestName, SuiteName)
{
  // your logic here...
  return success;
}
```

## Properties

TestPP is different from most C++ unit testing frameworks because it also
supports **properties**: invariants that hold true for your algorithms.

To use properties, `#include <property.h>`.

Properties are also declared with macros and function bodies, slightly
differently.

```cpp
DECLARE_PROPERTY(StringReverse, Algos, const string& s)
{
  string r(s);
  reverse(r.begin(), r.end());
  reverse(r.begin(), r.end());
  return s == r;
}
```

The third argument to `DECLARE_PROPERTY` is the function argument. TestPP will
generate arbitrary values of this type and feed them to your property checker.

## Arbitrary

TestPP knows how to generate values of standard types, but if you have a type
that you've defined, you may need to tell TestPP how to generate it. To do this,
specialize the `Arbitrary` template. Examples are in `arbitrary_*.h`.

`Arbitrary` supplies two functions: `generate` which returns a single value, and
`shrink` which takes a value, and returns a vector of values based on it. If
TestPP finds that a property fails to hold for a given value, it will call
`shrink` in an attempt to find the smallest test case that breaks the property.
