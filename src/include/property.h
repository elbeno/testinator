#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include <iostream>
#include <memory>

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

    bool check(std::size_t N, bool quiet, unsigned long int randomSeed)
    {
      return m_internal->check(N, quiet, randomSeed);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual bool check(std::size_t N, bool quiet, unsigned long int randomSeed) = 0;
    };

    template <typename U>
    struct Internal : public InternalBase
    {
      using paramType = std::decay_t<typename function_traits<U>::argType>;

      Internal(const U& u) : m_u(u) {}

      virtual bool check(std::size_t N, bool quiet, unsigned long int randomSeed)
      {
        m_failedResults.clear();
        m_failedSeeds.clear();

        checkInternal(N, randomSeed);

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

      void checkInternal(std::size_t N, unsigned long int randomSeed)
      {
        for (std::size_t i = 0; i < N; ++i)
        {
          unsigned long int seed = randomSeed;
          if (seed == 0)
          {
            std::random_device rd;
            seed = rd();
          }
          paramType p = Arbitrary<paramType>::generate(N, seed);
          if (!checkSingle(p))
          {
            m_failedSeeds.push_back(seed);
            return;
          }
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

#include "test.h"

//------------------------------------------------------------------------------
#define DECLARE_PROPERTY(NAME, SUITE, ARG)                 \
  class SUITE##NAME##Property : public testpp::Test        \
  {                                                        \
  public:                                                  \
    SUITE##NAME##Property()                                \
      : testpp::Test(#NAME "Property", #SUITE)             \
      , m_numChecks(1)                                     \
    {}                                                     \
    virtual bool Setup(const testpp::RunParams& params)    \
    {                                                      \
      m_numChecks = params.m_numPropertyChecks;            \
      m_quiet = (params.m_flags & testpp::QUIET_SUCCESS) != 0;  \
      m_randomSeed = params.m_randomSeed;                  \
      return true;                                         \
    }                                                      \
    virtual bool Run()                                     \
    {                                                      \
      testpp::Property p(*this);                           \
      if (!m_quiet)                                        \
        std::cout << m_name << ": ";                       \
      return p.check(m_numChecks, m_quiet, m_randomSeed);  \
    }                                                      \
    bool operator()(ARG) const;                            \
    size_t m_numChecks;                                    \
    bool m_quiet;                                          \
    unsigned long int m_randomSeed;                        \
  } s_##SUITE##NAME##_Property;                            \
  bool SUITE##NAME##Property::operator()(ARG) const
