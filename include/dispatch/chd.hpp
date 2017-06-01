#pragma once

#include "dispatch/detail/chd.hpp"
#include "dispatch/utilities/sequence.hpp"

namespace dispatch {

  template<template<typename...> typename IntermediateHash, typename ...Inputs>
  struct CHDHash {
    static constexpr std::size_t set_size = sizeof...(Inputs);
    static constexpr bool use_fallback = set_size <= 4;

    template<typename RuntimeType>
    static constexpr auto hash(const RuntimeType& input) {
      if constexpr (!use_fallback) {
        // TODO: len(G) != set_size, so this differs from the Python impl
        std::size_t key = detail::distinct_hash(0, input, set_size);
        const auto [status, d] = second_hash(key);
        switch(status) {
          case detail::hash_status::Unique:
            return d;
          case detail::hash_status::Collision:
            return detail::distinct_hash(d, input, set_size);  // TODO: len(V)?
          case detail::hash_status::Empty:
          default:
            return set_size;
        }
      } else {
        return second_hash(input);
      }
    }

    template<typename RuntimeType>
    constexpr auto operator()(const RuntimeType& input) const {
      return hash(input);
    }

  private:
    static constexpr auto second_hash =
        detail::construct_hash<IntermediateHash, Inputs...>();

    static constexpr bool no_collisions() {
      return unique(std::index_sequence<hash(Inputs{})...>{});
    }

    static_assert(no_collisions());
  };

  template<template<typename...> typename Hash, typename... Args,
      typename = std::enable_if_t<(Constant<Args>() && ...)>>
  static constexpr auto make_chd() {
    return CHDHash<Hash, Args...>{};
  }

  template<typename... Args,
      typename = std::enable_if_t<(Constant<Args>() && ...)>>
  static constexpr auto make_chd() {
    return CHDHash<SwitchTable, Args...>{};
  }

  template<template<typename...> typename Hash, typename A, typename... Args,
      typename = std::enable_if_t<Constant<A>() && (Constant<Args>() && ...)>>
  static constexpr auto make_chd(A&&, Args&&...) {
    return CHDHash<Hash, std::decay_t<A>, std::decay_t<Args>...>{};
  }

  template<typename A, typename... Args,
      typename = std::enable_if_t<Constant<A>() && (Constant<Args>() && ...)>>
  static constexpr auto make_chd(A&&, Args&&...) {
    return CHDHash<SwitchTable, std::decay_t<A>, std::decay_t<Args>...>{};
  }

}  // namespace dispatch
