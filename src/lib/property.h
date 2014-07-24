#pragma once

#include "arbitrary.h"
#include <iostream>

namespace testpp
{

  //------------------------------------------------------------------------------
  template <typename T>
  class Property
  {
    typedef T paramType;

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
      m_failedResults.clear();
      checkInternal(N);
      std::for_each(m_failedResults.begin(), m_failedResults.end(), [&] (const paramType& p) {
          if (!quiet)
            std::cout << "Failed: " << p << std::endl;
        });
      return (m_failedResults.empty());
    }

  private:
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
      if (!m_internal->call(p))
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

    struct InternalBase
    {
      virtual ~InternalBase() {}
      virtual bool call(const paramType& p) const = 0;
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
      Internal(const U& u) : m_u(u) {}

      virtual bool call(const paramType& p) const
      {
        return m_u(p);
      }

      U m_u;
    };

    // specialization for std::function
    template <typename U>
    struct Internal<U, true> : public InternalBase
    {
      Internal(const U* u) : m_u(u) {}

      virtual bool call(const paramType& p) const
      {
        return m_u(p);
      }

      U* m_u;
    };

    InternalBase* m_internal;
    std::vector<paramType> m_failedResults;
  };

}
