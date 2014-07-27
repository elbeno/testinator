#pragma once

#include "arbitrary.h"
#include "function_traits.h"
#include <iostream>

namespace testpp
{

  //------------------------------------------------------------------------------
  class Property
  {
  public:
    template <typename F>
    Property(const F& f)
    {
      m_internal = new Internal<F>(f);
    }

    ~Property()
    {
      delete m_internal;
    }

    bool check(std::size_t N = 100, bool quiet = true)
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
      typedef function_traits<U> traits;
      typedef typename std::remove_cv<
        typename std::remove_reference<
          typename traits::argType>::type>::type paramType;

      Internal(const U& u) : m_u(u) {}

      virtual bool check(std::size_t N, bool quiet)
      {
        m_failedResults.clear();
        checkInternal(N);
        std::for_each(m_failedResults.begin(), m_failedResults.end(), [&] (const paramType& p) {
            std::cout << "Failed: " << p << std::endl;
          });

        if (!m_failedResults.empty())
          std::cout << N << " checks, " << m_failedResults.size() << " failures." << std::endl;
        else if (!quiet)
          std::cout << N << " checks passed." << std::endl;

        return (m_failedResults.empty());
      }

      void checkInternal(std::size_t N)
      {
        for (std::size_t i = 0; i < N; ++i)
        {
          paramType p = Arbitrary<paramType>::generate(N);
          if (!checkSingle(p))
          {
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
    };

    InternalBase* m_internal;
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
      return true;                                         \
    }                                                      \
    virtual bool Run()                                     \
    {                                                      \
      testpp::Property p(*this);                           \
      if (!m_quiet)                                        \
        std::cout << m_name << ": ";                       \
      return p.check(m_numChecks, m_quiet);                \
    }                                                      \
    bool operator()(ARG) const;                            \
    size_t m_numChecks;                                    \
    bool m_quiet;                                          \
  } s_##SUITE##NAME##_Property;                            \
  bool SUITE##NAME##Property::operator()(ARG) const
