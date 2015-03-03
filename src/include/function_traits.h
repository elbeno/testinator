#pragma once

namespace testpp
{

  //------------------------------------------------------------------------------
  template <typename T>
  struct function_traits
    : public function_traits<decltype(&T::operator())>
  {};

  template <typename R, typename A>
  struct function_traits<R(A)>
  {
    using argType = A;
  };

  template <typename C, typename R, typename A>
  struct function_traits<R(C::*)(A)>
    : public function_traits<R(A)>
  {};

  template <typename C, typename R, typename A>
  struct function_traits<R(C::*)(A) const>
    : public function_traits<R(A)>
  {};

}
