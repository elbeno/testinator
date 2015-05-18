# Testinator

Experiments with testing in C++. Testinator is a unit testing framework. It is
known to work with:
 * Clang 3.4/libc++
 * GCC 4.9/libstdc++

### Status
[![Build Status](https://travis-ci.org/elbeno/testinator.svg?branch=master)](https://travis-ci.org/elbeno/testinator)

Testinator is distributed under the MIT license. See LICENSE for details.

## Basic setup

Testinator is a header-only library. To use it in the simplest way, a complete
program is:

```cpp
#define TESTINATOR_MAIN
#include <testinator.h>
```

This includes everything you need and provides a main function with some command
line parameters:

```
Usage: testinator [OPTION]...
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

Ordinary unit tests are grouped into suites and defined with a macro
(arguments: test name and suite name) and function body which should return
`true` if the test is successful, `false` otherwise.

```cpp
DEF_TEST(TestName, SuiteName)
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

## Critical failures

Ordinarily, a test should return `true` or `false` to indicate success or
failure. If a test has a problem that means it should be skipped, you can use
`SKIP` to skip that test:

```cpp
DEF_TEST(TestName, SuiteName)
{
  // this test went wrong in some deep way
  SKIP("Something bad happened");
  return true;
}
```

And if a test has such a problem that no other tests are likely to succeed,
you can use `ABORT` to bail out completely:

```cpp
DEF_TEST(TestName, SuiteName)
{
  // critical failure, don't run any more tests
  ABORT("Self destruct");
  return true;
}
```

After `SKIP` or `ABORT` it doesn't matter what is returned.

## Branches

Instead of exposing fixtures or setup/teardown functionality, Testinator uses
the idea of Branches. Branches are subsections of tests that will be executed on
successive runs of the test. This allows fixtures to be replaced with more
natural scoping. Branches are introduced with the `BRANCH` macro and their
name may be retrieved with `BRANCH_NAME`. For example:

```cpp
DEF_TEST(TestName, SuiteName)
{
  // some common setup here...
  bool success = true;

  BRANCH(A)
  {
    // this executes first time around
    DIAGNOSTIC("In branch " << BRANCH_NAME);
  }

  BRANCH(B)
  {
    // this executes second time around
    DIAGNOSTIC("In branch " << BRANCH_NAME);
  }

  return success;
}
```

Branches may be nested if further common structure is required; the test will
run as many times as necessary to visit all the "leaves". If Branches are not
explicitly named, `BRANCH_NAME` will be automatically provided with filename and
line information.

If a test returns early (assumedly with a failure, returning `false`) from
within a Branch, this will cause subsequent Branches to be skipped.

## Properties

Testinator also supports **properties**: invariants that hold true for your
algorithms.

Properties are defined the same way as tests, just with a different macro and
additional parameters that are the function arguments. (One argument is the
usual case, shown here.)

```cpp
DEF_PROPERTY(StringReverse, Algos, const string& s)
{
  string r(s);
  reverse(r.begin(), r.end());
  reverse(r.begin(), r.end());
  return s == r;
}
```

Testinator will generate arbitrary values of the arguments and feed them to your
test.

## Arbitrary

Testinator knows how to generate values of standard types, but if you have a
type that you've defined, you may need to tell Testinator how to generate it. To
do this, specialize the `Arbitrary` template. Examples are in `arbitrary_*.h`
and `property.cpp`. The output operator, `operator<<`, should also be available
for your type.

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

If Testinator finds that a property fails to hold for a given value, it will
call `shrink` in an attempt to find the smallest test case that breaks the
property. For example, a test on a string that breaks if 'A' is present may
produce:

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
DEF_TIMED_TEST(TestName, SuiteName)
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
complexity" value (`ORDER_1`, `ORDER_LOG_N`, `ORDER_N`, `ORDER_N_LOG_N`, or
`ORDER_N2`). Once again, there is no return value because the test exists only
to measure complexity.

```cpp
DEF_COMPLEXITY_PROPERTY(ThisIsOrderN, Complexity, ORDER_N, string&& s)
{
  max_element(s.begin(), s.end());
}
```

Generating values for use in complexity properties will call `generate_n` on the
`Arbitrary` class.

When measuring complexity, timing is of course important. If the function is
very small and optimized by the compiler, Testinator may not be able to
accurately measure the time.

If the complexity test comes in at (or *under*) the expected complexity, it will
be considered a pass.

## Output Formatters

The default output formatter uses ANSI coloring (use `--nocolor` to turn it off)
and normally omits output from passing tests (use `--verbose` to show the output
of all tests).

An output formatter for the Test Anything Protocol is also available (use
`--output=TAP`).
