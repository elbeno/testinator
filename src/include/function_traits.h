#pragma once

namespace testpp
{

  //------------------------------------------------------------------------------
  template <typename T>
  struct function_traits
    : public function_traits<decltype(&T::operator())>
  {};

  template <typename R, typename Arg>
  struct function_traits<R(Arg)>
  {
    typedef Arg argType;
  };

  template <typename C, typename R, typename Arg>
  struct function_traits<R(C::*)(Arg)>
    : public function_traits<R(Arg)>
  {};

  template <typename C, typename R, typename Arg>
  struct function_traits<R(C::*)(Arg) const>
    : public function_traits<R(Arg)>
  {};

}
