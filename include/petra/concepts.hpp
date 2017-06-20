// Copyright Jacqueline Kay 2017
// Distributed under the MIT License.
// See accompanying LICENSE.md or https://opensource.org/licenses/MIT

#pragma once
#include <experimental/type_traits>
#include <iostream>
#include <tuple>

#ifdef PETRA_ENABLE_CPP14
#include <boost/hana/if.hpp>
#endif  // PETRA_ENABLE_CPP14

namespace petra {

#ifdef PETRA_ENABLE_CPP14
  template<typename...T>
  using void_t = std::experimental::void_t<T...>;
#else
  template<typename...T>
  using void_t = std::void_t<T...>;
#endif

  template<template<typename...> class Op, typename... Args>
  using is_detected = std::experimental::is_detected<Op, Args...>;

  template<typename T, typename S>
  using comparable_t = decltype(std::declval<std::decay_t<T>>()
                                == std::declval<std::decay_t<S>>());

  template<typename T, typename S>
  static constexpr bool Comparable() {
    return is_detected<comparable_t, T, S>{};
  }

  // TupleAccess
  // TODO: generalize to non-std-tuples, customization point
  template<typename T>
  using tuple_access_t =
      // decltype(std::get<std::declval<std::size_t>()>(std::declval<T>()));
      // Workaround for libstdc++, declval can't be used here
      decltype(std::get<0>(std::declval<T>()));

  template<typename T>
  static constexpr bool TupleAccess() {
    return is_detected<tuple_access_t, std::decay_t<T>>{};
  }
  template<typename T>
  using pair_access_t = void_t<decltype(std::declval<T>().first),
                                    decltype(std::declval<T>().second)>;

  template<typename T>
  static constexpr bool PairAccess() {
    return is_detected<pair_access_t, std::decay_t<T>>{};
  }

  // Constant
  template<typename T>
  using data_accessor_t = decltype(T::value);

  template<typename T>
  static constexpr bool Constant() {
#ifdef PETRA_ENABLE_CPP14
    namespace hana = boost::hana;
    return hana::if_(is_detected<data_accessor_t, T>{},
      []() {
        return Comparable<T, decltype(T::value)>();
      },
      []() {
        return false;
      }
    );
#else
    if constexpr (is_detected<data_accessor_t, T>{}) {
      return Comparable<T, decltype(T::value)>();
    }
    return false;
#endif
  }

}  // namespace petra
