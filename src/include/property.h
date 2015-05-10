// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include "prettyprint.h"
#include "test.h"
#include "test_macros.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

namespace testinator
{

  //------------------------------------------------------------------------------
  class Property
  {
  public:
    template <typename F>
    Property(const F& f)
      : m_internal(std::make_unique<Internal<F>>(f))
    {
    }

    bool check(std::size_t N, const Outputter* outputter)
    {
      return m_internal->check(N, outputter);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual bool check(std::size_t N,
                         const Outputter*) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      using argTuple = typename function_traits<U>::argTuple;

      Internal(const U& u) : m_u(u) {}

      virtual bool check(std::size_t N, const Outputter* op)
      {
        auto seed = m_u.m_randomSeed;
        for (std::size_t i = 0; i < N; ++i)
        {
          auto t = Arbitrary<argTuple>::generate(N, seed);
          if (!checkSingle(std::move(t), op))
          {
            op->diagnostic(
                Diagnostic(Cons<Nil>()
                           << "Reproduce failure with --seed=" << seed));
            return false;
          }
          seed = GetTestRegistry().RNG()();
        }
        return true;
      }

      bool checkSingle(argTuple&& t, const Outputter* op)
      {
        if (function_traits<U>::apply(m_u, t)) return true;

        op->diagnostic(
            Diagnostic(Cons<Nil>()
                       << "Failed " << prettyprint(t)));

        std::vector<argTuple> v = Arbitrary<argTuple>::shrink(std::move(t));
        if (!v.empty())
        {
          return std::all_of(std::make_move_iterator(v.begin()),
                             std::make_move_iterator(v.end()),
                             [this, op] (argTuple&& st)
                             { return checkSingle(std::move(st), op); });
        }
        return false;
      }

      U m_u;
    };

    std::unique_ptr<InternalBase> m_internal;
  };

  //------------------------------------------------------------------------------
  class PropertyTest : public Test
  {
  public:
    PropertyTest(const std::string& n, const std::string& s)
      : Test(n, s)
    {}

    virtual bool Setup(const RunParams& params) override
    {
      m_numChecks = params.m_numPropertyChecks;
      m_randomSeed = params.m_randomSeed;
      if (m_randomSeed == 0)
      {
        std::random_device rd;
        m_randomSeed = rd();
      }
      GetTestRegistry().RNG().seed(m_randomSeed);
      return true;
    }

    size_t m_numChecks = 1;
    unsigned long m_randomSeed = 0;
  };
}

//------------------------------------------------------------------------------
#define DEF_PROPERTY(NAME, SUITE, ...)                          \
  class SUITE##NAME##Property : public testinator::PropertyTest \
  {                                                             \
  public:                                                       \
    SUITE##NAME##Property()                                     \
      : testinator::PropertyTest(#NAME "Property", #SUITE) {}   \
    virtual bool Run() override                                 \
    {                                                           \
      testinator::Property p(*this);                            \
      return p.check(m_numChecks, m_op);                        \
    }                                                           \
    bool operator()(__VA_ARGS__);                               \
  } s_##SUITE##NAME##_Property;                                 \
  bool SUITE##NAME##Property::operator()(__VA_ARGS__)
