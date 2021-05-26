//
// Created by marcel on 24.05.21.
//

#ifndef FICTION_TRAITS_HPP
#define FICTION_TRAITS_HPP

#include <mockturtle/traits.hpp>

namespace fiction
{

#pragma region has_is_wire
template <class Ntk, class = void>
struct has_is_wire : std::false_type
{};

template <class Ntk>
struct has_is_wire<Ntk, std::void_t<decltype(std::declval<Ntk>().is_wire(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_wire_v = has_is_wire<Ntk>::value;
#pragma endregion

#pragma region has_is_buf
template <class Ntk, class = void>
struct has_is_buf : std::false_type
{};

template <class Ntk>
struct has_is_buf<Ntk, std::void_t<decltype(std::declval<Ntk>().is_buf(std::declval<mockturtle::node<Ntk>>()))>>
        : std::true_type
{};

template <class Ntk>
inline constexpr bool has_is_buf_v = has_is_buf<Ntk>::value;
#pragma endregion

// TODO is_topology_network

}  // namespace fiction

#endif  // FICTION_TRAITS_HPP
