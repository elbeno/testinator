#pragma once

#include <ostream>
#include <utility>

namespace testpp
{

  // -----------------------------------------------------------------------------
  // We will build up the expression as a list of types.

  template <typename L>
  struct Capture
  {
    typedef L type;
    L m_list;
  };

  struct Nothing {};

  // -----------------------------------------------------------------------------
  // We will recurse into the expression to extract the LHS, RHS and the RelOp
  // linking the two sides.

  template <typename T>
  struct LHSRecur
  {
    constexpr static auto value(T&& t) noexcept
    {
      return t;
    }
  };

  template <typename T>
  struct RHSRecur
  {
    constexpr static auto value(T&& t) noexcept
    {
      return t;
    }
  };

  template <typename T>
  struct RelOpRecur
  {
    constexpr static auto value(T&&) noexcept
    {
      return "";
    }
  };

  template <typename T>
  struct EvalRecur
  {
    constexpr static auto value(T&& t) noexcept
    {
      return t;
    }
  };

  // -----------------------------------------------------------------------------
  // The first op we use is just to capture the top level on the RHS of the ->*

  struct SecondOp
  {
    template <typename T, typename U>
    constexpr static U compute(T&&, U&& u) noexcept
    {
      return u;
    }
  };

  // -----------------------------------------------------------------------------
  // Now, operations that we can compute by recursion into the expression.

  struct AddOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) + u;
    }
  };

  struct SubOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) - u;
    }
  };

  struct MulOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) * u;
    }
  };

  struct DivOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) / u;
    }
  };

  struct ModOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) % u;
    }
  };

  struct LShiftOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) << u;
    }
  };

  struct RShiftOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) >> u;
    }
  };

  // -----------------------------------------------------------------------------
  // Here are relational operations that link the LHS and RHS.

  // -----------------------------------------------------------------------------
  struct EQOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) == u;
    }
  };

  struct NEOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) != u;
    }
  };

  struct LTOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) < u;
    }
  };

  struct GTOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) > u;
    }
  };

  struct LTEOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) <= u;
    }
  };

  struct GTEOp
  {
    template <typename T, typename U>
    constexpr static auto compute(T&& t, U&& u)
    {
      return LHSRecur<T>::value(std::forward<T>(t)) >= u;
    }
  };

  // -----------------------------------------------------------------------------
  // OpPair uses the phantom operator type and stores the pair of operands

  template <class H, class Op, class T>
  struct OpPair
  {
    constexpr OpPair(H&& h, T&& t)
      : m_pair(std::forward<H>(h), std::forward<T>(t))
    {}

    std::pair<H,T> m_pair;
  };

  // -----------------------------------------------------------------------------
  // Now we have the operator overloads to capture each type.

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, SecondOp, T>> operator->*(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  // -----------------------------------------------------------------------------
  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, AddOp, T>> operator+(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, SubOp, T>> operator-(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, MulOp, T>> operator*(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, DivOp, T>> operator/(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, ModOp, T>> operator%(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  // -----------------------------------------------------------------------------
  // Overloading << and >> lowers their precedence to below relational ops. If we
  // don't overload them, it's an error, but if we overload them, the compiler
  // complains about the lowered precedence wrt relational ops and requires parens
  // around the << operands, which means these functions doesn't actually get
  // called. But they need to exist.

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, LShiftOp, T>> operator<<(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, RShiftOp, T>> operator>>(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  // -----------------------------------------------------------------------------
  // Now, the relational ops.

  // -----------------------------------------------------------------------------
  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, EQOp, T>> operator==(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, NEOp, T>> operator!=(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, LTOp, T>> operator<(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, GTOp, T>> operator>(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, LTEOp, T>> operator<=(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  template <typename H, typename T>
  constexpr inline Capture<OpPair<H, GTEOp, T>> operator>=(Capture<H>&& head, T&& tail)
  {
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};
  }

  // -----------------------------------------------------------------------------
  // Functions to extract the LHS, RHS and the relational op linking the sides.

  template <typename T>
  constexpr inline auto LHS(T&& c)
  {
    using L = typename std::remove_reference_t<T>::type;
    return LHSRecur<L>::value(std::forward<L>(c.m_list));
  }

  template <typename T>
  constexpr inline auto RHS(T&& c)
  {
    using L = typename std::remove_reference_t<T>::type;
    return RHSRecur<L>::value(std::forward<L>(c.m_list));
  }

  template <typename T>
  constexpr inline auto RelOp(T&& c)
  {
    using L = typename std::remove_reference_t<T>::type;
    return RelOpRecur<L>::value(std::forward<L>(c.m_list));
  }

  template <typename T>
  constexpr inline auto Eval(T&& c)
  {
    using L = typename std::remove_reference_t<T>::type;
    return EvalRecur<L>::value(std::forward<L>(c.m_list));
  }

  // -----------------------------------------------------------------------------
  // Recursing into the LHS. Compute the binary ops, and return the LHS for any
  // relational ops.

  template <typename H, typename Op, typename T>
  struct LHSRecur<OpPair<H, Op, T>>
  {
    constexpr static auto value(OpPair<H, Op, T>&& t)
    {
      return Op::compute(std::forward<H>(t.m_pair.first),
                         LHSRecur<T>::value(std::forward<T>(t.m_pair.second)));
    }
  };

  template <typename H, typename T>
  struct LHSRecur<OpPair<H, EQOp, T>>
  {
    constexpr static auto value(OpPair<H, EQOp, T>&& t)
    {
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first));
    }
  };

  template <typename H, typename T>
  struct LHSRecur<OpPair<H, NEOp, T>>
  {
    constexpr static auto value(OpPair<H, NEOp, T>&& t)
    {
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first));
    }
  };

  template <typename H, typename T>
  struct LHSRecur<OpPair<H, LTOp, T>>
  {
    constexpr static auto value(OpPair<H, LTOp, T>&& t)
    {
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first));
    }
  };

  template <typename H, typename T>
  struct LHSRecur<OpPair<H, GTOp, T>>
  {
    constexpr static auto value(OpPair<H, GTOp, T>&& t)
    {
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first));
    }
  };

  template <typename H, typename T>
  struct LHSRecur<OpPair<H, LTEOp, T>>
  {
    constexpr static auto value(OpPair<H, LTEOp, T>&& t)
    {
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first));
    }
  };

  template <typename H, typename T>
  struct LHSRecur<OpPair<H, GTEOp, T>>
  {
    constexpr static auto value(OpPair<H, GTEOp, T>&& t)
    {
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first));
    }
  };

  // -----------------------------------------------------------------------------
  // Recursing into the RHS. Just return the RHS.

  template <typename H, typename Op, typename T>
  struct RHSRecur<OpPair<H, Op, T>>
  {
    constexpr static auto value(OpPair<H, Op, T>&& t)
    {
      return RHSRecur<T>::value(std::forward<T>(t.m_pair.second));
    }
  };

  // -----------------------------------------------------------------------------
  // Recursing to find the relational op. Discard the LHS until we hit a
  // relational op.

  template <typename H, typename Op, typename T>
  struct RelOpRecur<OpPair<H, Op, T>>
  {
    constexpr static auto value(OpPair<H, Op, T>&& t)
    {
      return RelOpRecur<T>::value(std::forward<T>(t.m_pair.second));
    }
  };

  template <typename H, typename T>
  struct RelOpRecur<OpPair<H, EQOp, T>>
  {
    constexpr static auto value(OpPair<H, EQOp, T>&&)
    {
      return "==";
    }
  };

  template <typename H, typename T>
  struct RelOpRecur<OpPair<H, NEOp, T>>
  {
    constexpr static auto value(OpPair<H, NEOp, T>&&)
    {
      return "!=";
    }
  };

  template <typename H, typename T>
  struct RelOpRecur<OpPair<H, LTOp, T>>
  {
    constexpr static auto value(OpPair<H, LTOp, T>&&)
    {
      return "<";
    }
  };

  template <typename H, typename T>
  struct RelOpRecur<OpPair<H, GTOp, T>>
  {
    constexpr static auto value(OpPair<H, GTOp, T>&&)
    {
      return ">";
    }
  };

  template <typename H, typename T>
  struct RelOpRecur<OpPair<H, LTEOp, T>>
  {
    constexpr static auto value(OpPair<H, LTEOp, T>&&)
    {
      return "<=";
    }
  };

  template <typename H, typename T>
  struct RelOpRecur<OpPair<H, GTEOp, T>>
  {
    constexpr static auto value(OpPair<H, GTEOp, T>&&)
    {
      return ">=";
    }
  };

  // -----------------------------------------------------------------------------
  // Recursing to evaluate the expression.

  template <typename H, typename Op, typename T>
  struct EvalRecur<OpPair<H, Op, T>>
  {
    constexpr static auto value(OpPair<H, Op, T>&& t)
    {
      return Op::compute(std::forward<H>(t.m_pair.first),
                         EvalRecur<T>::value(std::forward<T>(t.m_pair.second)));
    }
  };

}
