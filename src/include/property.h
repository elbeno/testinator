#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include "test.h"

#include <iostream>
#include <memory>
#include <random>

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

    bool check(std::size_t N, bool quiet)
    {
      return m_internal->check(N, quiet);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual bool check(std::size_t N, bool quiet) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      using paramType = std::decay_t<typename function_traits<U>::argType>;

      Internal(const U& u) : m_u(u) {}

      virtual bool check(std::size_t N, bool quiet)
      {
        m_failedResults.clear();
        m_failedSeeds.clear();

        checkInternal(N);

        for (size_t i = 0; i < m_failedResults.size(); ++i)
        {
          std::cout << "Failed: " << m_failedResults[i];
          if (i < m_failedSeeds.size())
          {
            std::cout << " (seed=" << m_failedSeeds[i] << ")";
          }
          std::cout << std::endl;
        }

        if (!m_failedResults.empty())
          std::cout << N << " checks, " << m_failedResults.size() << " failures." << std::endl;
        else if (!quiet)
          std::cout << N << " checks passed." << std::endl;

        return (m_failedResults.empty());
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
        if (!m_u(p))
        {
          m_failedResults.push_back(p);
          std::vector<paramType> v = Arbitrary<paramType>::shrink(p);
          if (!v.empty())
            return checkMulti(v);
          return false;
        }
        return true;
      }

      bool checkMulti(const std::vector<paramType>& v)
      {
        bool pass = true;
        std::for_each(v.begin(), v.end(), [&] (const paramType& p) {
            pass = pass && checkSingle(p);
          });
        return pass;
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
      m_quiet = (params.m_flags & testpp::QUIET_SUCCESS) != 0;
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
    bool m_quiet = false;
    unsigned long m_randomSeed = 0;
  };
}

//------------------------------------------------------------------------------
#define DECLARE_PROPERTY(NAME, SUITE, ARG)                  \
  class SUITE##NAME##Property : public testpp::PropertyTest \
  {                                                         \
  public:                                                   \
    SUITE##NAME##Property()                                 \
      : testpp::PropertyTest(#NAME "Property", #SUITE)      \
    {}                                                      \
    virtual bool Run() override                             \
    {                                                       \
      testpp::Property p(*this);                            \
      if (!m_quiet)                                         \
        std::cout << m_name << ": ";                        \
      return p.check(m_numChecks, m_quiet);                 \
    }                                                       \
    bool operator()(ARG) const;                             \
  } s_##SUITE##NAME##_Property;                             \
  bool SUITE##NAME##Property::operator()(ARG) const
