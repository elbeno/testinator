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

    bool check(std::size_t N = 100, bool quiet = false)
    {
      return m_internal->check(N, quiet);
    }

  private:
    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual bool check(std::size_t N = 100, bool quiet = false) = 0;
    };

    template <typename U, bool = std::is_function<U>::value>
    struct Internal : public InternalBase
    {
      Internal(const U&) {}
    };

    // specialization for an actual function
    template <typename U>
    struct Internal<U, false> : public InternalBase
    {
      typedef function_traits<U> traits;
      typedef typename std::remove_cv<
        typename std::remove_reference<
          typename traits::template arg<0>::type>::type>::type paramType;

      Internal(const U& u) : m_u(u) {}

      virtual bool check(std::size_t N = 100, bool quiet = false)
      {
        m_failedResults.clear();
        checkInternal(N);
        std::for_each(m_failedResults.begin(), m_failedResults.end(), [&] (const paramType& p) {
            if (!quiet)
              std::cout << "Failed: " << p << std::endl;
          });
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

    // specialization for std::function
    template <typename U>
    struct Internal<U, true> : public InternalBase
    {
      typedef function_traits<U> traits;
      typedef typename std::remove_cv<
        typename std::remove_reference<
          typename traits::template arg<0>::type>::type>::type paramType;

      Internal(const U* u) : m_u(u) {}

      virtual bool check(std::size_t N = 100, bool quiet = false)
      {
        m_failedResults.clear();
        checkInternal(N);
        std::for_each(m_failedResults.begin(), m_failedResults.end(), [&] (const paramType& p) {
            if (!quiet)
              std::cout << "Failed: " << p << std::endl;
          });
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

      U* m_u;
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
      return true;                                         \
    }                                                      \
    virtual bool Run()                                     \
    {                                                      \
      testpp::Property p(*this);                           \
      return p.check(m_numChecks);                         \
    }                                                      \
    bool operator()(ARG);                                  \
    size_t m_numChecks;                                    \
  } s_##SUITE##NAME##_Property;                            \
  bool SUITE##NAME##Property::operator()(ARG)
