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
specialize the `Arbitrary` template. Examples are in `arbitrary_*.h`. The output
operator, `operator<<`, should also be available for your type.

`Arbitrary` supplies two functions: `generate` which returns a single value, and
`shrink` which takes a value, and returns a vector of values based on it. If
TestPP finds that a property fails to hold for a given value, it will call
`shrink` in an attempt to find the smallest test case that breaks the property.

For types that don't make sense to shrink, `shrink` should return an empty
vector. It should also return an empty vector if the argument has been shrunk
enough.

Examples of usage can be found in `property.cpp`.

## Timing

Sometimes you want to time tests. You can set up a timed test easily, just like
a regular test. There is no return value, because it's assumed the test exists
so you can see what time it takes.

To use timed tests, `#include <timed_test.h>`.

```cpp
DECLARE_TIMED_TEST(TestName, SuiteName)
{
  // your logic here...
}
```

And you will see (verbose) output like:

`Simple: 100 tests run in 1206146ns (12061 ns per test).`

## Complexity

Sometimes timing isn't enough, and what you really want to test is: what is the
complexity of my algorithm? You might want to do this to get some kind of
guarantee your algorithm will scale, for example. You can use an algorithmic
complexity property, similar to a property, but with an extra "expected
complexity".

To use complexity properties, `#include <complexity.h>`.

```cpp
DECLARE_COMPLEXITY_PROPERTY(ThisIsOrderN, Complexity, const string& s, ORDER_N)
{
  max_element(s.begin(), s.end());
}
```

If the test comes in *under* the expected complexity, it will be considered a
pass, but the anomaly will be reported.
