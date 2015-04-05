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
  class Region
  {
    bool m_complete;
    bool m_canRunChild;
    std::list<Region> m_children;
    int m_id;
    std::string m_name;

  public:
    Region(int id, const char* name)
      : m_complete(false)
      , m_canRunChild(true)
      , m_id(id)
      , m_name(name)
    {
      if (m_name.empty())
      {
        std::ostringstream oss;
        oss << "(" __FILE__ ":" << std::to_string(id) << ")";
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

    Region& pushChild(int id, const char* name)
    {
      auto i = std::find_if(m_children.begin(), m_children.end(),
                            [=] (const Region& r) { return r.m_id == id; });
      if (i != m_children.end()) return *i;

      m_children.emplace_back(id, name);
      return m_children.back();
    }

    static std::stack<Region*>& getStack()
    {
      static std::stack<Region*> s;
      return s;
    }

    static Region& currentParent()
    {
      return *getStack().top();
    }
  };

  class RegionScope
  {
  public:
    RegionScope(int id, const char* name)
      : m_parent(Region::currentParent())
      , m_child(m_parent.pushChild(id, name))
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
        Region::getStack().push(&m_child);
        m_child.setComplete(true);
      }
    }

    ~RegionScope()
    {
      m_parent.childComplete(m_child.isComplete());
      m_parent.setCanRunChild(true);
    }

    bool canRun() const { return m_canRun; }

  private:
    Region& m_parent;
    Region& m_child;
    bool m_canRun;
  };
}

#define TESTINATOR_XCAT(a, b) a ## b
#define TESTINATOR_CAT(a, b) TESTINATOR_XCAT(a, b)
#define TESTINATOR_UNIQUE_NAME(base) TESTINATOR_CAT(base, __LINE__)

#define DEF_REGION(...)                                                 \
  testinator::RegionScope TESTINATOR_UNIQUE_NAME(rs)(__LINE__, #__VA_ARGS__); \
  if (TESTINATOR_UNIQUE_NAME(rs).canRun())                              \
    if (auto TESTINATOR_UNIQUE_NAME(rspop) = testinator::at_scope_exit( \
            [] () { testinator::Region::getStack().pop(); }))

#define REGION_NAME (testinator::Region::getStack().top()->getName())

namespace testinator
{
  inline bool Test::RunWithRegions()
  {
    Region root(-1, "(root)");
    Region::getStack().push(&root);
    auto TESTINATOR_UNIQUE_NAME(rootpop) = at_scope_exit(
        [] () { Region::getStack().pop(); });
    while (!root.isComplete())
    {
      root.setComplete(true);
      if (!Run())
        return false;
    }
    return true;
  }
}
