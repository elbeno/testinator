#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include "test.h"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace testpp
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
      using paramType = std::decay_t<typename function_traits<U>::argType>;

      Internal(const U& u) : m_u(u) {}

      virtual bool check(std::size_t N,
                         const Outputter* op)
      {
        m_failedResults.clear();
        m_failedSeeds.clear();

        checkInternal(N);

        for (size_t i = 0; i < m_failedResults.size(); ++i)
        {
          std::ostringstream s;
          s << "Failed: " << m_failedResults[i];
          if (i < m_failedSeeds.size())
          {
            s << " (seed=" << m_failedSeeds[i] << ")";
          }
          op->diagnostic(s.str());
        }

        if (!m_failedResults.empty())
        {
          std::ostringstream s;
          s << N << " checks, " << m_failedResults.size() << " failures.";
          op->diagnostic(s.str());
        }

        return m_failedResults.empty();
      }

      void checkInternal(std::size_t N)
      {
        auto seed = m_u.m_randomSeed;
        for (std::size_t i = 0; i < N; ++i)
        {
          paramType p = Arbitrary<paramType>::generate(N, seed);
          if (!checkSingle(p))
          {
            m_failedSeeds.push_back(seed);
            return;
          }
          seed = GetTestRegistry().RNG()();
        }
      }

      bool checkSingle(const paramType& p)
      {
        if (m_u(p)) return true;

        m_failedResults.push_back(p);
        std::vector<paramType> v = Arbitrary<paramType>::shrink(p);
        if (!v.empty())
        {
          return std::all_of(v.cbegin(), v.cend(),
                             [this] (const paramType& pt)
                             { return checkSingle(pt); });
        }
        return false;
      }

      U m_u;
      std::vector<paramType> m_failedResults;
      std::vector<unsigned long int> m_failedSeeds;
    };

    std::unique_ptr<InternalBase> m_internal;
  };

}

namespace testpp
{
  class PropertyTest : public Test
  {
  public:
    PropertyTest(const std::string& name, const std::string& suiteName)
      : Test(name, suiteName)
    {}

    virtual bool Setup(const testpp::RunParams& params) override
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
#define DECLARE_PROPERTY(NAME, SUITE, ARG)                  \
  class SUITE##NAME##Property : public testpp::PropertyTest \
  {                                                         \
  public:                                                   \
    SUITE##NAME##Property()                                 \
      : testpp::PropertyTest(#NAME "Property", #SUITE) {}   \
    virtual bool Run() override                             \
    {                                                       \
      testpp::Property p(*this);                            \
      return p.check(m_numChecks, m_op);                    \
    }                                                       \
    bool operator()(ARG) const;                             \
  } s_##SUITE##NAME##_Property;                             \
  bool SUITE##NAME##Property::operator()(ARG) const
