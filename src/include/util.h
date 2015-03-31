#pragma once

#include <algorithm>

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
}
