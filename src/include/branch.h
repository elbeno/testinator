// Copyright (c) 2014, 2015 Ben Deane
// This code is distributed under the MIT license. See LICENSE for details.

#pragma once

#include <algorithm>
#include <list>
#include <sstream>
#include <stack>
#include <string>

namespace testinator
{
  //------------------------------------------------------------------------------
  template <typename F>
  class AtScopeExit
  {
  public:
    AtScopeExit(const F& f) : m_f(f) {}
    AtScopeExit(F&& f) : m_f(std::move(f)) {}
    ~AtScopeExit() { m_f(); }
    operator bool() const { return true; }
  private:
    F m_f;
  };

  template <typename F>
  AtScopeExit<F> at_scope_exit(F&& f)
  {
    return AtScopeExit<F>(std::forward<F>(f));
  }

  //------------------------------------------------------------------------------
  class Branch
  {
    bool m_complete;
    bool m_canRunChild;
    std::list<Branch> m_children;
    int m_id;
    std::string m_name;

  public:
    Branch(int line, const char* file, const char* name)
      : m_complete(false)
      , m_canRunChild(true)
      , m_id(line)
      , m_name(name)
    {
      if (m_name.empty())
      {
        std::ostringstream oss;
        oss << "(" << file << ":" << std::to_string(line) << ")";
        m_name = oss.str();
      }
    }

    bool isComplete() const
    {
      return m_complete;
    }

    bool canRunChild() const
    {
      return m_canRunChild;
    }

    const std::string& getName() const
    {
      return m_name;
    }

    void setComplete(bool b)
    {
      m_complete = b;
    }

    void childComplete(bool b)
    {
      m_complete = m_complete && b;
    }

    void setCanRunChild(bool b)
    {
      m_canRunChild = b;
    }

    Branch& pushChild(int line, const char* file, const char* name)
    {
      auto i = std::find_if(m_children.begin(), m_children.end(),
                            [=] (const Branch& r) { return r.m_id == line; });
      if (i != m_children.end()) return *i;

      m_children.emplace_back(line, file, name);
      return m_children.back();
    }

    static std::stack<Branch*>& getStack()
    {
      static std::stack<Branch*> s;
      return s;
    }

    static Branch& currentParent()
    {
      return *getStack().top();
    }
  };

  class BranchScope
  {
  public:
    BranchScope(int line, const char* file, const char* name)
      : m_parent(Branch::currentParent())
      , m_child(m_parent.pushChild(line, file, name))
    {
      m_canRun = m_parent.canRunChild();
      if (!m_canRun)
      {
        m_parent.setComplete(false);
        return;
      }
      m_canRun = !m_child.isComplete();
      if (m_canRun)
      {
        m_parent.setCanRunChild(false);
        Branch::getStack().push(&m_child);
        m_child.setComplete(true);
      }
    }

    ~BranchScope()
    {
      m_parent.childComplete(m_child.isComplete());
      m_parent.setCanRunChild(true);
    }

    bool canRun() const { return m_canRun; }

  private:
    Branch& m_parent;
    Branch& m_child;
    bool m_canRun;
  };
}

#define TESTINATOR_XCAT(a, b) a ## b
#define TESTINATOR_CAT(a, b) TESTINATOR_XCAT(a, b)
#define TESTINATOR_UNIQUE_NAME(base) TESTINATOR_CAT(base, __LINE__)

// From http://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
// Used to work around MSVC preprocessor __VA_ARGS__ implementation issues.
#define TESTINATOR_CREATE_2(dummy, x) x
#define TESTINATOR_CREATE_1(x) TESTINATOR_CREATE_2(,x)
#define TESTINATOR_CREATE_0() TESTINATOR_CREATE_1("")
#define TESTINATOR_FUNC_CHOOSER(_f1, _f2, _f3, ...) _f3
#define TESTINATOR_FUNC_RECOMPOSER(argsWithParen) TESTINATOR_FUNC_CHOOSER argsWithParen
#define TESTINATOR_CHOOSE_FROM_ARG_COUNT(...) TESTINATOR_FUNC_RECOMPOSER((__VA_ARGS__, TESTINATOR_CREATE_1, ))
#define TESTINATOR_NO_ARG_EXPANDER() ,,TESTINATOR_CREATE_0
#define TESTINATOR_MACRO_CHOOSER(...) TESTINATOR_CHOOSE_FROM_ARG_COUNT(TESTINATOR_NO_ARG_EXPANDER __VA_ARGS__ ())

#define BRANCH(...)                                                                 \
  testinator::BranchScope TESTINATOR_UNIQUE_NAME(rs)(                               \
       __LINE__, __FILE__, TESTINATOR_MACRO_CHOOSER(dummy, #__VA_ARGS__)(#__VA_ARGS__));   \
  if (TESTINATOR_UNIQUE_NAME(rs).canRun())                                          \
    if (auto TESTINATOR_UNIQUE_NAME(rspop) = testinator::at_scope_exit(             \
            [] () { testinator::Branch::getStack().pop(); }))

#define BRANCH_NAME (testinator::Branch::getStack().top()->getName())

namespace testinator
{
  inline bool Test::RunWithBranches()
  {
    Branch root(-1, "", "(root)");
    Branch::getStack().push(&root);
    auto TESTINATOR_UNIQUE_NAME(rootpop) = at_scope_exit(
        [] () { Branch::getStack().pop(); });
    while (!root.isComplete())
    {
      root.setComplete(true);
      if (!Run())
        return false;
    }
    return true;
  }
}
