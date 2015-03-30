# TestPP

Experiments with testing in C++. TestPP is a unit testing framework.

## Basic setup

TestPP is a header-only library. To use it in the simplest way, a complete
program is:

```cpp
#define TESTPP_MAIN
#include <testpp.h>
```

This includes everything you need and provides a main function with some command
line parameters:

```
Usage: testpp [OPTION]...
Run all tests in randomized order by default.

--testName=NAME    run only the named test
--suiteName=NAME   run only the tests in the named suite
--alpha            run tests in alphabetical order
--output=FORMAT    use the specified output formatter, e.g. TAP
--verbose          give verbose output (according to formatter)
--nocolor          output without ANSI color codes (according to formatter)
--numChecks=N      number of checks to use for property tests
--seed=SEED        use SEED for property test randomization
```

## Simple usage

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

You can use the `EXPECT` macro to test conditions in a way that will output the
condition on failure:

```cpp
bool b = true;
EXPECT(!b == b);
```

Produces the output:

```
EXPECT FAILED: <file>:<line> (!b == b => false == true)
```

The `DIAGNOSTIC` macro produces arbitrary diagnostic output:

```cpp
DIAGNOSTIC("Hello world " << 42);
```

## Properties

TestPP also supports **properties**: invariants that hold true for your
algorithms.

Properties are declared the same way as tests, just with a different macro and an additional parameter that is the function argument.

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
generate arbitrary values of this type and feed them to your test function.

## Arbitrary

TestPP knows how to generate values of standard types, but if you have a type
that you've defined, you may need to tell TestPP how to generate it. To do this,
specialize the `Arbitrary` template. Examples are in `arbitrary_*.h` and
`property.cpp`. The output operator, `operator<<`, should also be available for
your type.

`Arbitrary` supplies three static functions:

* `generate` which returns a single value according to a loose notion of
  "generation", allowing you to alter the "size" of the generated value

* `generate_n`, which is similar, but with a stricter definition of size -- see
  [Complexity](#complexity) below

* `shrink` which takes a value, and returns a vector of values based on it. For
  types that don't make sense to shrink, `shrink` should return an empty vector.
  It should also return an empty vector if the argument has been shrunk enough.

Both `generate` and `generate_n` take an argument that will be used to seed an
RNG. On failure, the failing seed will be reported so that you can reproduce the
test.

If TestPP finds that a property fails to hold for a given value, it will call
`shrink` in an attempt to find the smallest test case that breaks the property.
For example, a test on a string that breaks if 'A' is present may produce:

```
Failed: ~+9Sbh~"D q`A%:-\_+G
Failed: q`A%:-\_+G
Failed: q`A%:
Failed: A%:
Failed: A
Reproduce failure with --seed=1419143051
FAIL: BrokenStringProperty
```

Examples of usage can be found in `property.cpp`.

## Timing

Sometimes you want to time tests. You can set up a timed test easily, just like
a regular test. There is no return value, because it's assumed the test exists
so you can see what time it takes.

```cpp
DECLARE_TIMED_TEST(TestName, SuiteName)
{
  // your logic here...
}
```

And you will see output like:

`TestName: 100 tests run in 1206146ns (12061 ns per test).`

## Complexity

Sometimes timing isn't enough, and what you really want to test is: what is the
complexity of my algorithm? You might want to do this to get some kind of
guarantee your algorithm will scale, for example. You can use an algorithmic
complexity property, similar to a property, but with an extra "expected
complexity".

```cpp
DECLARE_COMPLEXITY_PROPERTY(ThisIsOrderN, Complexity, const string& s, ORDER_N)
{
  max_element(s.begin(), s.end());
}
```

Generating values for use in complexity properties will call `generate_n` on the
`Arbitrary` class.

If the complexity test comes in *under* the expected complexity, it will be
considered a pass.

## Output Formatters

The default output formatter uses ANSI coloring (use `--nocolor` to turn it off)
and normally omits output from passing tests (use `--verbose` to show the output
of all tests).

An output formatter for the Test Anything Protocol is also available (use
`--output=TAP`).
